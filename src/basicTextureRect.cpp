#include "basicTextureRect.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

using namespace UI;

BasicTextureRect::~BasicTextureRect()
{
  if (m_texture)
    SDL_DestroyTexture(m_texture);

  SDL_DestroySurface(m_surface);
}

void BasicTextureRect::draw(SDL_Renderer *renderer)
{
  SDL_RenderTexture(renderer, m_texture, nullptr, &m_texture_space);
}

void BasicTextureRect::HandleResizeEvent(const SDL_FRect &space)
{
  m_available_space = space;

  applyMargin(m_available_space);

  if (!m_texture)
    return;

  // don't do aspect ratio calculation
  if (!m_keep_ratio)
  {
    m_texture_space = m_available_space;
    return;
  }

  const float image_aspect_ratio =
      static_cast<float>(m_texture->w) / static_cast<float>(m_texture->h);

  const float viewport_aspect_ratio =
      static_cast<float>(space.w) / static_cast<float>(space.h);

  // setting the corner
  m_texture_space.x = m_available_space.x;
  m_texture_space.y = m_available_space.y;

  // if image aspect ratio is BIGGER than viewport -> adjust to width
  if (image_aspect_ratio >= viewport_aspect_ratio)
  {
    m_texture_space.w = m_available_space.w;
    m_texture_space.h = m_available_space.w / image_aspect_ratio;
  }
  else
  {
    m_texture_space.w = m_available_space.h * image_aspect_ratio;
    m_texture_space.h = m_available_space.h;
  }
}

void BasicTextureRect::setTexture(SDL_Renderer *renderer, SDL_Surface *surface,
                                  SDL_ScaleMode scalemode)
{
  if (m_texture)
    SDL_DestroyTexture(m_texture);

  SDL_DestroySurface(m_surface);

  m_surface = surface;

  m_texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_SetTextureScaleMode(m_texture, scalemode);

  HandleResizeEvent(m_available_space);
}
