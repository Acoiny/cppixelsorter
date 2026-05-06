#include "gui.hpp"
#include "Sorters/baseImageSorter.hpp"
#include "Ui/UiManager.hpp"
#include "Ui/checkbox.hpp"
#include "Ui/container/hbox.hpp"
#include "Ui/container/vbox.hpp"
#include "Ui/logger.hpp"
#include "Ui/slider.hpp"
#include "Ui/textBox.hpp"
#include "Ui/textButton.hpp"
#include "filepicker.hpp"

#include "imageData.hpp"
#include "sortTask.hpp"
#include "stb_image.h"
#include "textureRect.hpp"
#include "timer.hpp"

#include <SDL3/SDL.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_surface.h>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>

#include "embed_data.hpp"

static void Error(const std::string &msg)
{
  UI::Logger::Fatal("Error: {} - {}", msg, SDL_GetError());
};

Gui::Gui(int width, int height, const std::string &title)
{
  if (!SDL_SetAppMetadata(title.c_str(), "0.0.1", "com.cpp.pixelsorter"))
    throw std::runtime_error(
        std::format("Unable to set metadata: {}", SDL_GetError()));

  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    Error("Unable to initialize SDL3");
  }
  if (!TTF_Init())
  {
    Error("Unable to initialize SDL3_ttf");
  }
  SDL_CreateWindowAndRenderer(title.c_str(), width, height,
                              SDL_WINDOW_RESIZABLE, &m_window, &m_renderer);

  // setting the window icon
  {
    auto stream = SDL_IOFromConstMem(assets_icon_png, assets_icon_png_len);
    auto icon = SDL_LoadSurface_IO(stream, true);
    SDL_SetWindowIcon(m_window, icon);
    SDL_DestroySurface(icon);
  }

  m_isRunning = true;

  m_uiManager = std::make_unique<UI::UiManager>(m_renderer);

  // setting up filepicker callback
  m_filepicker.onSelect = [this](const std::string &filename, bool saving)
  {
    if (saving)
      SaveImage(filename);
    else
      LoadImage(filename);
  };

  // TODO: properly construct elements!
  auto hb = m_uiManager->addElement<UI::HBox>();
  {
    constexpr float DEFAULT_MARGIN = 5;
    auto vb = hb->addElement<UI::VBox>();

    auto load_btn = vb->addElement<UI::TextButton>("Load");
    load_btn->onLeftClick = [this]() { m_filepicker.open(); };
    load_btn->SetMargin(DEFAULT_MARGIN);

    auto sort_btn = vb->addElement<UI::TextButton>("Sort");
    sort_btn->onLeftClick = std::bind(&Gui::ThreadedSort, this);
    sort_btn->SetMargin(DEFAULT_MARGIN);

    auto save_btn = vb->addElement<UI::TextButton>("Save");
    save_btn->onLeftClick = [this]() { m_filepicker.open(true); };
    save_btn->SetMargin(DEFAULT_MARGIN);

    // hue-sliders
    {
      auto minSliderBox = vb->addElement<UI::HBox>();

      // min slider elements
      auto minSlider = minSliderBox->addElementFrac<UI::Slider<int>>(3, 0, 360);
      minSlider->SetMargin({.right = 10, .left = 10});
      auto minSliderText = minSliderBox->addElement<UI::TextBox>("Min: 0°");

      // hue texture
      {
        auto hue_box = vb->addElement<UI::HBox>();
        auto huebar_texture = hue_box->addElementFrac<TextureRect>(3);
        huebar_texture->setKeepRatio(false).SetMargin(
            {.top = 10, .bottom = 100});

        hue_box->addElementFrac<UI::TextBox>(1, "");

        auto stream = SDL_IOFromConstMem(assets_huebar, assets_huebar_len);
        auto surf = SDL_LoadSurface_IO(stream, true);

        huebar_texture->setTexture(m_renderer, surf, SDL_SCALEMODE_LINEAR);
        huebar_texture->SetMargin({.right = 10, .left = 10});
      }

      auto maxSliderBox = vb->addElement<UI::HBox>();
      auto maxSlider =
          maxSliderBox->addElementFrac<UI::Slider<int>>(3, 0, 360, 360);
      maxSlider->SetMargin({.right = 10, .left = 10});
      auto maxSliderText = maxSliderBox->addElement<UI::TextBox>("Max: 360°");

      std::weak_ptr<UI::TextBox> w_minSliderText = minSliderText;
      std::weak_ptr<UI::Slider<int>> w_maxSlider = maxSlider;

      // capturing weak pointers
      minSlider->onValueChange = [this, w_minSliderText, w_maxSlider](int value)
      {
        if (auto text = w_minSliderText.lock())
          text->setText(std::format("Min: {:>3}°", value));

        m_slider_value.min = value;

        if (m_slider_value.min > m_slider_value.max)
        {
          if (auto ms = w_maxSlider.lock())
            ms->SetValue(value, true);
        }

        if (m_autosort)
          ThreadedSort();
      }; // std::bind_front(&Gui::SliderChanged, this);

      std::weak_ptr<UI::TextBox> w_maxSliderText = maxSliderText;
      std::weak_ptr<UI::Slider<int>> w_minSlider = minSlider;

      maxSlider->onValueChange = [this, w_maxSliderText, w_minSlider](int value)
      {
        if (auto text = w_maxSliderText.lock())
          text->setText(std::format("Max: {:>3}°", value));

        m_slider_value.max = value;

        if (m_slider_value.max < m_slider_value.min)
        {
          if (auto ms = w_minSlider.lock())
            ms->SetValue(value, true);
        }

        if (m_autosort)
          ThreadedSort();
      };
    }

    // spacer
    vb->addElementFrac<UI::TextBox>(9, "");

    m_infoText = vb->addElementFrac<UI::TextBox>(1, "");

    // autosorting checkbox
    {
      auto hb = vb->addElementFrac<UI::HBox>(1);
      auto chBox = hb->addElementFrac<UI::CheckBox>(1, false);
      chBox->onChange = [this](bool value) { m_autosort = value; };
      chBox->SetMargin(DEFAULT_MARGIN);

      hb->addElementFrac<UI::TextBox>(3, "Autosort");
    }
  }

  m_texturerect = hb->addElementFrac<TextureRect>(5);
  // "sending" a resize event to update UI layout
  {
    SDL_Event fake_event;
    fake_event.type = SDL_EVENT_WINDOW_RESIZED;
    fake_event.window.data1 = width;
    fake_event.window.data2 = height;
    m_uiManager->handleEvent(fake_event);
  }

  m_thread = std::jthread(
      [this](std::stop_token token)
      {
        // run, until stop is requested
        while (!token.stop_requested())
        {
          std::unique_lock lock(m_thread_data.mutex);
          // wait, until the main thread signals sorting task, or ending
          m_thread_data.cv.wait(lock,
                                [this, &token]()
                                {
                                  return m_thread_data.task != nullptr ||
                                         token.stop_requested();
                                });

          // if stop is requested, we quit
          if (token.stop_requested())
            return;

          std::unique_ptr<SortTask> task(std::move(m_thread_data.task));
          lock.unlock();

          BaseImageSorter sorter(*task); // yes yes, that's illegal... TODO: fix
          ImageData result = sorter.RunTask();

          lock.lock();
          m_thread_data.result_image = std::move(result);
          m_thread_data.state = State::DONE;
          lock.unlock();
        }
      });
}

Gui::~Gui()
{
  // unload original image
  // UnloadImage();
  m_uiManager.reset();

  SDL_Quit();
  TTF_Quit();

  m_thread.request_stop();
  m_thread_data.cv.notify_one();
}

void Gui::Update()
{
  // if sorting is done, get the thread's result
  if (m_thread_data.state == State::DONE)
  {
    {
      std::lock_guard lock(m_thread_data.mutex);
      m_thread_data.state = State::IDLE;
      m_sorted_image = std::move(m_thread_data.result_image);
    }
    m_texturerect->setTexture(m_renderer, m_sorted_image.toSurface(),
                              SDL_SCALEMODE_NEAREST, false);
  }

  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    if (event.type == SDL_EVENT_QUIT)
    {
      m_isRunning = false;
    }
    else if (m_uiManager->isEventRelevant(event))
    {
      m_uiManager->handleEvent(event);
    }
  }

  // SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
  SDL_SetRenderDrawColor(m_renderer, 93, 59, 107, 255);
  SDL_RenderClear(m_renderer);

  m_uiManager->draw(m_renderer);

  SDL_RenderPresent(m_renderer);
  SDL_Delay(20);
}

void Gui::LoadImage(const std::string &path)
{
  m_original_image = ImageData(path);
  m_texturerect->setTexture(m_renderer, m_original_image.toSurface());
}

void Gui::RunSort()
{
  if (!m_original_image.pixels)
  {
    UI::Logger::Warn("Load an image before sorting!");
    return;
  }

  // passing NULL is fine
  m_sorted_image = m_original_image;

  if (!m_sorted_image.pixels)
  {
    UI::Logger::Error("Unable to copy image");
    return;
  }

  SortTask task{
      .image = m_sorted_image,
      .hue_values = {.min = m_slider_value.min, .max = m_slider_value.max}};

  BaseImageSorter sorter(task);
  Timer t;
  sorter.RunTask();
  auto msg = std::format("Sorting took {}", t.get());
  UI::Logger::Info("{}", msg);
  m_infoText->setText(msg);

  m_texturerect->setTexture(m_renderer, m_sorted_image.toSurface());
}

void Gui::ThreadedSort()
{
  // first, set new task using the mutex
  {
    std::lock_guard lock(m_thread_data.mutex);
    m_thread_data.task = std::make_unique<SortTask>(SortTask{
        .image = m_original_image,
        .hue_values = {.min = m_slider_value.min, .max = m_slider_value.max}});
  }

  // now wake up thread
  m_thread_data.cv.notify_one();
}

void Gui::SaveImage(const std::string &filename)
{
  if (m_sorted_image.pixels == nullptr && m_original_image.pixels == nullptr)
  {
    UI::Logger::Warn("No image to save!");
    return;
  }

  // save original, if not sorted yet
  try
  {
    m_sorted_image.write_to_file(filename);
  }
  catch (std::runtime_error &e)
  {
    UI::Logger::Error("{}", e.what());
  }
}
