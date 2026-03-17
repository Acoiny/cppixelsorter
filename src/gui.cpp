#include "gui.hpp"
#include "Ui/textButton.hpp"
#include "Ui/textManager.hpp"
#include "imageSorter.hpp"

#include "stb_image.h"

#include <SDL3/SDL.h>

#include <SDL3_ttf/SDL_ttf.h>
#include <memory>
#include <print>

Gui::Gui(int width, int height, const std::string &title)
{
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  SDL_SetAppMetadata(title.c_str(), "0.0.1", "com.cpp.pixelsorter");
  SDL_CreateWindowAndRenderer("Pixelsorter", width, height,
                              SDL_WINDOW_RESIZABLE, &m_window, &m_renderer);

  m_isRunning = true;

  UI::TextManager::Init(m_renderer);

  {
    auto btn = std::make_shared<UI::TextButton>(10, 10, "Sort");
    btn->onLeftClick = [&]()
    {
      ImageSorter sorter(m_surface);
      sorter.sort_vertical(0);
      // sorter.write_to_file("file_for_ruben.png");
      LoadTextureFromSurface(m_surface);
    };
    m_uiElements.emplace_back(btn);
  }
  {
    auto btn = std::make_shared<UI::TextButton>(10, 50, "Save");
    btn->onLeftClick = [&]()
    {
      ImageSorter sorter(m_surface);
      sorter.write_to_file("output.png");
    };
    m_uiElements.emplace_back(btn);
  }
}

Gui::~Gui()
{
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
    else
    {
      for (const auto el : m_uiElements)
      {
        if (el->HandleEvent(event))
          break;
      }
    }
  }

  SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
  SDL_RenderClear(m_renderer);

  SDL_FRect blankspace = {0, 0, 200, 600};
  SDL_RenderFillRect(m_renderer, &blankspace);

  if (m_texture)
  {
    const auto rect = get_image_ratio_rect(m_texture->w, m_texture->h, 0.25f);
    SDL_RenderTexture(m_renderer, m_texture, nullptr, &rect);
  }

  for (const auto el : m_uiElements)
  {
    el->draw(m_renderer);
  }

  SDL_RenderPresent(m_renderer);
  SDL_Delay(20);
}

void Gui::LoadTextureFromSurface(SDL_Surface *surface)
{
  SDL_Surface *newSurface =
      SDL_ConvertSurface(m_surface, SDL_PIXELFORMAT_RGB24);

  // destroy old surface
  SDL_DestroySurface(m_surface);

  // if conversion failed, don't load image
  if (!newSurface)
  {
    m_surface = nullptr;
    std::println(stderr, "Unable to create texture!");
  }
  else
  {
    m_surface = newSurface;
  }

  m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
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
    LoadTextureFromSurface(m_surface);
  }
  else
  {
    std::println(stderr, "Unable to load image {}!", path);
  }

  std::println("Needs lock: {}", SDL_MUSTLOCK(m_surface));
}

SDL_FRect Gui::get_image_ratio_rect(int image_width, int image_height,
                                    float empty_space_percent)
{

  SDL_Rect viewport;

  if (!SDL_GetRenderViewport(m_renderer, &viewport))
  {
    std::println(stderr, "Error: {}", SDL_GetError());
    return {};
  }

  const float start_x = viewport.w * empty_space_percent;

  SDL_FRect res = {.x = start_x, .y = 0, .w = 0, .h = 0};

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
