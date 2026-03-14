#include "gui.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>

#include <print>

Gui::Gui(int width, int height, const std::string &title)
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_SetAppMetadata(title.c_str(), "0.0.1", "com.cpp.pixelsorter");
  SDL_CreateWindowAndRenderer("Pixelsorter", width, height,
                              SDL_WINDOW_RESIZABLE, &m_window, &m_renderer);

  m_isRunning = true;
}

Gui::~Gui() { SDL_Quit(); }

void Gui::Update()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    if (event.type == SDL_EVENT_QUIT)
      m_isRunning = false;
  }

  SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
  SDL_RenderClear(m_renderer);

  SDL_FRect blankspace = {0, 0, 200, 600};
  SDL_RenderFillRect(m_renderer, &blankspace);

  if (m_texture)
  {
    const auto rect = get_image_ratio_rect(m_texture->w, m_texture->h);
    SDL_RenderTexture(m_renderer, m_texture, nullptr, &rect);
  }

  SDL_RenderPresent(m_renderer);
  SDL_Delay(20);
}

void Gui::LoadImage(const std::string &path)
{
  m_surface = IMG_Load(path.c_str());

  if (m_surface)
  {
    SDL_Surface *newSurface =
        SDL_ConvertSurface(m_surface, SDL_PIXELFORMAT_RGBA32);

    // destroy old surface
    SDL_DestroySurface(m_surface);

    // if conversion failed, don't load image
    if (!newSurface)
    {
      m_surface = nullptr;
    }
    else
    {
      m_surface = newSurface;
    }
  }

  if (m_surface)
  {
    m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
  }
}

SDL_FRect Gui::get_image_ratio_rect(int image_width, int image_height)
{
  SDL_FRect res = {0};

  SDL_Rect viewport;

  if (!SDL_GetRenderViewport(m_renderer, &viewport))
  {
    std::println(stderr, "Error: {}", SDL_GetError());
    return {};
  }

  const float image_aspect_ratio =
      static_cast<float>(image_width) / static_cast<float>(image_height);

  const float viewport_aspect_ratio =
      static_cast<float>(viewport.w) / static_cast<float>(viewport.h);

  // if image aspect ratio is BIGGER than viewport -> adjust to width
  if (image_aspect_ratio >= viewport_aspect_ratio)
  {
    res.w = viewport.w;
    res.h = viewport.w / image_aspect_ratio;
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
