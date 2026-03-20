#include "gui.hpp"
#include "Ui/UiManager.hpp"
#include "Ui/button.hpp"
#include "Ui/container/hbox.hpp"
#include "Ui/container/vbox.hpp"
#include "Ui/textBox.hpp"
#include "Ui/textButton.hpp"
#include "filepicker.hpp"
#include "imageSorter.hpp"

#include "stb_image.h"
#include "textureRect.hpp"
#include "timer.hpp"

#include <SDL3/SDL.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <cstdint>
#include <memory>
#include <print>
#include <stdexcept>

#include "icon_data.hpp"

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

  m_uiManager = std::make_unique<UI::UiManager>(m_renderer);

  m_isRunning = true;

  // TODO: properly construct elements!
  auto hb = m_uiManager->addElement<UI::HBox>();
  {
    auto vb = hb->addElement<UI::VBox>();
    vb->addElement<UI::TextButton>(0, 0, "Load")->onLeftClick =
        std::bind(&Gui::PickFile, this);

    vb->addElement<UI::TextButton>(0, 0, "Sort")->onLeftClick =
        std::bind(&Gui::RunSort, this);
    vb->addElement<UI::TextButton>(0, 0, "Save")->onLeftClick =
        std::bind(&Gui::SaveFile, this);

    // spacer
    vb->addElementFrac<UI::TextBox>(10, 0, 0, "");

    m_infoText = vb->addElementFrac<UI::TextBox>(1, 0, 0, "Time: ");
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
  return;
  // corner coordinates
  float c_x = 10, c_y = 10;

  // add text box to show current path
  m_fileName = m_uiManager->addElement<UI::TextBox>(c_x, c_y, "Path here");

  // add load button
  m_uiManager->addElement<UI::TextButton>(c_x, c_y + 40, 100, 30, "Load")
      ->onLeftClick = std::bind(&Gui::PickFile, this);

  // add "sort"-button
  m_uiManager->addElement<UI::TextButton>(c_x, c_y + 80, 100, 30, "Sort")
      ->onLeftClick = std::bind(&Gui::RunSort, this);

  // add "save"-button
  m_uiManager->addElement<UI::TextButton>(c_x, c_y + 120, 100, 30, "Save")
      ->onLeftClick = std::bind(&Gui::SaveFile, this);
}

Gui::~Gui()
{
  if (m_surface)
  {
    uint8_t *pixels = (uint8_t *)m_surface->pixels;
    SDL_DestroySurface(m_surface);
    stbi_image_free(pixels);
  }
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
  // Loading the surface with stb_image
  int w, h, channels;
  uint8_t *img = stbi_load(path.c_str(), &w, &h, &channels, 3);

  std::println("Image: {} {} - channels {}", w, h, channels);

  m_surface = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGB24, img, w * 3);
  // m_surface = IMG_Load(path.c_str());

  if (m_surface)
  {
    m_texturerect->setTexture(m_renderer, m_surface);
    // LoadTextureFromSurface(m_surface);
  }
  else
  {
    std::println(stderr, "Unable to load image {}!", path);
  }

  std::println("Needs lock: {}", SDL_MUSTLOCK(m_surface));
}

SDL_FRect Gui::get_image_ratio_rect(int image_width, int image_height,
                                    int start_x)
{

  SDL_Rect viewport;

  if (!SDL_GetRenderViewport(m_renderer, &viewport))
  {
    std::println(stderr, "Error: {}", SDL_GetError());
    return {};
  }

  SDL_FRect res = {.x = static_cast<float>(start_x), .y = 0, .w = 0, .h = 0};

  const float image_aspect_ratio =
      static_cast<float>(image_width) / static_cast<float>(image_height);

  const float viewport_aspect_ratio =
      static_cast<float>(viewport.w - start_x) / static_cast<float>(viewport.h);

  // if image aspect ratio is BIGGER than viewport -> adjust to width
  if (image_aspect_ratio >= viewport_aspect_ratio)
  {
    res.w = (viewport.w - start_x);
    res.h = (viewport.w - start_x) / image_aspect_ratio;
  }
  else
  {
    res.w = viewport.h * image_aspect_ratio;
    res.h = viewport.h;
  }

  // if (image_width > viewport.w)
  // {
  //   res.w = viewport.w;
  //   res.h = (viewport.w / static_cast<float>(image_width)) * image_height;
  // }

  // std::println("viewport size: {} {}", res.w, res.h);

  return res;
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
    std::println("ERROR: Unable to open file!");
  }
}

void Gui::RunSort()
{
  ImageSorter sorter(m_surface);
  Timer t;
  sorter.sort_vertical(0);
  auto msg = std::format("Sorting took {}", t.get());
  std::println("{}", msg);
  m_infoText->setText(msg);

  m_texturerect->setTexture(m_renderer, m_surface);
}
void Gui::SaveFile()
{
  ImageSorter sorter(m_surface);
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
      std::println(stderr, "{}", e.what());
    }
  }
  else
  {
    std::println("ERROR: Unable to save file!");
  }
}
