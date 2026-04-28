#include "textureRect.hpp"
#include "Ui/logger.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

void TextureRect::draw(SDL_Renderer *renderer)
{
  // if (m_texture)
  if (m_texture)
  {
    m_render_rect = {
        .x = 0,
        .y = 0,
        .w = (float)m_texture->w * m_zoom_factor,
        .h = (float)m_texture->h * m_zoom_factor,
    };
    SDL_RenderTexture(renderer, m_texture, &m_render_rect, &m_texture_space);
  }
  else
  {
    SDL_RenderTexture(renderer, m_texture, nullptr, &m_texture_space);
  }
}

TextureRect::~TextureRect()
{
  if (m_texture)
    SDL_DestroyTexture(m_texture);

  // passing NULL is fine
  SDL_DestroySurface(m_surface);
}

bool TextureRect::HandleMouseEvent(SDL_Event &event)
{
  using Log = UI::Logger;

  switch (event.type)
  {
  case SDL_EVENT_MOUSE_MOTION:
  {
    float mx = event.motion.x;
    float my = event.motion.y;
    bool intersects = mx >= m_texture_space.x &&
                      mx <= (m_texture_space.x + m_texture_space.w) &&
                      my >= m_texture_space.y &&
                      my <= (m_texture_space.y + m_texture_space.h);

    if (intersects)
    {
      m_relative_mouse_pos = {(mx - m_texture_space.x) / m_texture_space.w,
                              (my - m_texture_space.y) / m_texture_space.h};
    }

    // if previously focused, check if focus is lost
    if (m_focused)
    {
      // focus is lost
      if (!intersects)
      {
        m_focused = false;
      }
      // event is always, whether focus is lost or nothing happens
      return true;
    }
    // if not focused previously
    else
    {
      // AND now intersecting
      if (intersects)
      {
        // it is in focus
        m_focused = true;
        // and the event is handled
        return true;
      }
    }
    break;
  }
  case SDL_EVENT_MOUSE_WHEEL:
  {
    auto [mx, my] = m_relative_mouse_pos;
    Log::Info("Mouse: {} {}", mx, my);

    // if not focused, ignore
    if (!m_focused)
      break;
    // TODO: check mouse position and zoom there
    // event.wheel.mouse_x
    if (event.wheel.integer_y > 0)
    {
      m_zoom_factor *= 0.9;
      if (m_zoom_factor < 0.1)
        m_zoom_factor = 0.1;
    }
    else
    {
      m_zoom_factor *= 1.1;
      if (m_zoom_factor > 1.0)
        m_zoom_factor = 1.0;
    }
    UI::Logger::Debug("Zoom: {}", m_zoom_factor);
    return true;
  }
  default:
    break;
  }
  return false;
}

void TextureRect::HandleResizeEvent(const SDL_FRect &space)
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

void TextureRect::setTexture(SDL_Renderer *renderer, SDL_Surface *surface,
                             SDL_ScaleMode scaleMode)
{
  if (m_texture)
    SDL_DestroyTexture(m_texture);

  SDL_DestroySurface(m_surface);

  m_surface = surface;

  m_texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_SetTextureScaleMode(m_texture, scaleMode);
  HandleResizeEvent(m_available_space);
}
