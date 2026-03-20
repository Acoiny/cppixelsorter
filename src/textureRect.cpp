#include "textureRect.hpp"
#include <SDL3/SDL_render.h>

void TextureRect::draw(SDL_Renderer *renderer)
{
  // if (m_texture)
  SDL_RenderTexture(renderer, m_texture, nullptr, &m_rect);
}

TextureRect::~TextureRect()
{
  if (m_texture)
    SDL_DestroyTexture(m_texture);
}

void TextureRect::HandleResizeEvent(const SDL_FRect &space)
{
  if (!m_texture)
    return;

  const float image_aspect_ratio =
      static_cast<float>(m_texture->w) / static_cast<float>(m_texture->h);

  const float viewport_aspect_ratio =
      static_cast<float>(space.w) / static_cast<float>(space.h);

  // setting the corner
  m_rect.x = space.x;
  m_rect.y = space.y;

  // if image aspect ratio is BIGGER than viewport -> adjust to width
  if (image_aspect_ratio >= viewport_aspect_ratio)
  {
    m_rect.w = space.w;
    m_rect.h = space.w / image_aspect_ratio;
  }
  else
  {
    m_rect.w = space.h * image_aspect_ratio;
    m_rect.h = space.h;
  }
}

void TextureRect::setTexture(SDL_Renderer *renderer, SDL_Surface *surface)
{
  if (m_texture)
    SDL_DestroyTexture(m_texture);
  m_texture = SDL_CreateTextureFromSurface(renderer, surface);
}
