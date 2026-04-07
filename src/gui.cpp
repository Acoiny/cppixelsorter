#include "gui.hpp"
#include "Sorters/baseImageSorter.hpp"
#include "Ui/UiManager.hpp"
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

#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>

#include "embed_data.hpp"

Gui::Gui(int width, int height, const std::string &title)
{
  if (!SDL_SetAppMetadata(title.c_str(), "0.0.1", "com.cpp.pixelsorter"))
    throw std::runtime_error(
        std::format("Unable to set metadata: {}", SDL_GetError()));

  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
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

  // TODO: properly construct elements!
  auto hb = m_uiManager->addElement<UI::HBox>();
  {
    auto vb = hb->addElement<UI::VBox>();

    vb->addElement<UI::TextButton>("Load")->onLeftClick =
        std::bind(&Gui::PickFile, this);
    vb->addElement<UI::TextButton>("Sort")->onLeftClick =
        std::bind(&Gui::ThreadedSort, this);
    vb->addElement<UI::TextButton>("Save")->onLeftClick =
        std::bind(&Gui::SaveFile, this);
    m_sliderText = vb->addElement<UI::TextBox>("0");

    vb->addElement<UI::Slider<int>>(0, 360)->onValueChange =
        std::bind_front(&Gui::SliderChanged, this); // WTF???

    // spacer
    vb->addElementFrac<UI::TextBox>(9, "");

    m_infoText = vb->addElementFrac<UI::TextBox>(1, "");
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
  // get thread result
  if (m_thread_data.state == State::DONE)
  {
    {
      std::lock_guard lock(m_thread_data.mutex);
      m_thread_data.state = State::IDLE;
      m_sorted_image = std::move(m_thread_data.result_image);
    }
    m_texturerect->setTexture(m_renderer, m_sorted_image.toSurface());
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

void Gui::PickFile()
{
  Filepicker fp;
  if (fp.open(false))
  {
    auto name = fp.getFile();

    if (m_fileName)
      m_fileName->setText(name);

    LoadImage(name);
  }
  else
  {
    UI::Logger::Error("Unable to open file!");
  }
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

  SortTask task{.image = m_sorted_image, .hue_values = {.min = m_slider_value}};

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
        .image = m_original_image, .hue_values = {.min = m_slider_value}});
  }

  // now wake up thread
  m_thread_data.cv.notify_one();
}

void Gui::SaveFile()
{
  if (m_sorted_image.pixels == nullptr && m_original_image.pixels == nullptr)
  {
    UI::Logger::Warn("No image to save!");
    return;
  }

  // save original, if not sorted yet
  Filepicker fp;
  if (fp.open(true))
  {
    auto name = fp.getFile();

    try
    {
      m_sorted_image.write_to_file(name);
    }
    catch (std::runtime_error &e)
    {
      UI::Logger::Error("{}", e.what());
    }
  }
  else
  {
    UI::Logger::Error("Unable to save file!");
  }
}

void Gui::SliderChanged(int value)
{
  m_sliderText->setText(std::to_string(value));

  m_slider_value = value;

  // RunSort();
  ThreadedSort();
}
