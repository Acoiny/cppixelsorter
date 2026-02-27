#include "gui.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3_image/SDL_image.h>

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

  if (m_texture)
  {
    SDL_RenderTexture(m_renderer, m_texture, nullptr, nullptr);
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
    ;
  }
}
