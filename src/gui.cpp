#include "gui.hpp"
#include "Ui/UiManager.hpp"
#include "Ui/container/hbox.hpp"
#include "Ui/container/vbox.hpp"
#include "Ui/logger.hpp"
#include "Ui/slider.hpp"
#include "Ui/textBox.hpp"
#include "Ui/textButton.hpp"
#include "cliImageSorter.hpp"
#include "filepicker.hpp"

#include "stb_image.h"
#include "textureRect.hpp"
#include "timer.hpp"

#include <SDL3/SDL.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_surface.h>
#include <cstdint>
#include <memory>
#include <stdexcept>

#include "embed_data.hpp"

Gui::Gui(int width, int height, const std::string &title)
{
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  SDL_SetAppMetadata(title.c_str(), "0.0.1", "com.cpp.pixelsorter");
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
        std::bind(&Gui::RunSort, this);
    vb->addElement<UI::TextButton>("Save")->onLeftClick =
        std::bind(&Gui::SaveFile, this);
    m_sliderText = vb->addElement<UI::TextBox>("1");

    vb->addElement<UI::Slider<int>>(0, 360)->onValueChange =
        std::bind(&Gui::SliderChanged, this, std::placeholders::_1); // WTF???

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
}

Gui::~Gui()
{
  // unload original image
  UnloadImage();
  m_uiManager.reset();

  SDL_Quit();
  TTF_Quit();
}

void Gui::Update()
{
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
  UnloadImage();
  // Loading the surface with stb_image
  int w, h, channels;
  uint8_t *img = stbi_load(path.c_str(), &w, &h, &channels, 3);

  UI::Logger::Debug("Image: {} {} - channels {}", w, h, channels);

  m_original = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGB24, img, w * 3);
  // m_surface = IMG_Load(path.c_str());

  if (m_original)
  {
    m_texturerect->setTexture(m_renderer, m_original);
  }
  else
  {
    UI::Logger::Error("Unable to load image {}!", path);
  }
}

void Gui::UnloadImage()
{
  // unload original image
  if (m_original)
  {
    uint8_t *pixels = (uint8_t *)m_original->pixels;
    SDL_DestroySurface(m_original);
    stbi_image_free(pixels);
    m_original = nullptr;
  }
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
  if (!m_original)
  {
    UI::Logger::Warn("Load an image before sorting!");
    return;
  }

  // passing NULL is fine
  SDL_DestroySurface(m_sorted);

  m_sorted = SDL_ConvertSurface(m_original, SDL_PIXELFORMAT_RGB24);

  if (!m_sorted)
  {
    UI::Logger::Error("Unable to copy surface: {}", SDL_GetError());
    return;
  }

  CliImageSorter sorter(m_sorted);
  Timer t;
  sorter.sort_vertical(m_slider_value);
  auto msg = std::format("Sorting took {}", t.get());
  UI::Logger::Info("{}", msg);
  m_infoText->setText(msg);

  m_texturerect->setTexture(m_renderer, m_sorted);
}

void Gui::SaveFile()
{
  if (m_sorted == nullptr && m_original == nullptr)
  {
    UI::Logger::Warn("No image to save!");
    return;
  }

  // save original, if not sorted yet
  CliImageSorter sorter(m_sorted == nullptr ? m_original : m_sorted);
  Filepicker fp;
  if (fp.open(true))
  {
    auto name = fp.getFile();

    try
    {
      sorter.write_to_file(name);
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

  RunSort();
}
