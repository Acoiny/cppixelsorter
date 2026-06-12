#include "zoomableTextureRect.hpp"
#include "Ui/Colors.hpp"
#include "Ui/cursorManager.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <stdexcept>

void ZoomableTextureRect::draw(SDL_Renderer *renderer)
{
  auto [r, g, b, a] = UI::Color::BACKGROUND;
  SDL_SetRenderDrawColor(renderer, r, g, b, a);
  SDL_RenderFillRect(renderer, &m_available_space);

  if (m_texture)
  {
    SDL_RenderTexture(renderer, m_texture, &m_render_rect, &m_texture_space);
  }
  else
  {
    SDL_RenderTexture(renderer, m_texture, nullptr, &m_texture_space);
  }
}

std::pair<UI::EventResult, std::optional<std::shared_ptr<UI::BaseElement>>>
ZoomableTextureRect::HandleMouseEvent(SDL_Event &event)
{
  UI::EventResult handled = UI::EventResult::UNHANDLED;

  switch (event.type)
  {
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
  {
    if (m_state == State::FOCUSED)
    {
      m_state = State::DRAGGING;
      UI::CursorManager::SetCursor(UI::CursorManager::CursorStyle::MOVE);
      handled = UI::EventResult::HANDLED;
    }
    break;
  }
  case SDL_EVENT_MOUSE_BUTTON_UP:
  {
    float mx = event.motion.x;
    float my = event.motion.y;
    bool intersects = mx >= m_texture_space.x &&
                      mx <= (m_texture_space.x + m_texture_space.w) &&
                      my >= m_texture_space.y &&
                      my <= (m_texture_space.y + m_texture_space.h);

    if (m_state == State::DRAGGING)
    {
      if (intersects)
      {
        m_state = State::FOCUSED;
        UI::CursorManager::SetCursor(UI::CursorManager::CursorStyle::POINT);
        m_mouse_pos = {(mx - m_texture_space.x), (my - m_texture_space.y)};
        ;
      }
      else
      {
        m_state = State::IDLE;
        UI::CursorManager::SetCursor();
      }
      handled = UI::EventResult::HANDLED;
    }
  }
  case SDL_EVENT_MOUSE_MOTION:
  {
    switch (m_state)
    {
    case State::IDLE:
    {
      float mx = event.motion.x;
      float my = event.motion.y;
      bool intersects = mx >= m_texture_space.x &&
                        mx <= (m_texture_space.x + m_texture_space.w) &&
                        my >= m_texture_space.y &&
                        my <= (m_texture_space.y + m_texture_space.h);

      // cursor is NO(  over the texture
      if (intersects)
      {
        m_mouse_pos = {(mx - m_texture_space.x), (my - m_texture_space.y)};
        m_state = State::FOCUSED;
        UI::CursorManager::SetCursor(UI::CursorManager::CursorStyle::POINT);
        return {handled, {}};
      }
      break;
    }
    case State::FOCUSED:
    {
      float mx = event.motion.x;
      float my = event.motion.y;
      bool intersects = mx >= m_texture_space.x &&
                        mx <= (m_texture_space.x + m_texture_space.w) &&
                        my >= m_texture_space.y &&
                        my <= (m_texture_space.y + m_texture_space.h);

      // cursor is still above texture -> update position
      if (intersects)
      {
        m_mouse_pos = {(mx - m_texture_space.x), (my - m_texture_space.y)};
      }
      else
      {
        // cursor left texture -> Idle
        m_state = State::IDLE;
        UI::CursorManager::SetCursor();
      }
      handled = UI::EventResult::HANDLED;
      break;
    }
    case State::DRAGGING:
    {
      float mx = event.motion.x;
      float my = event.motion.y;
      std::pair<float, float> new_mouse_pos = {(mx - m_texture_space.x),
                                               (my - m_texture_space.y)};

      auto delta_x =
          (m_mouse_pos.first - new_mouse_pos.first) / m_texture_space.w;
      auto delta_y =
          (m_mouse_pos.second - new_mouse_pos.second) / m_texture_space.h;

      m_render_rect.x += delta_x * m_texture->w * m_zoom_factor;

      m_render_rect.y += delta_y * m_texture->h * m_zoom_factor;

      clampRenderRect();

      m_mouse_pos = new_mouse_pos;
      handled = UI::EventResult::HANDLED;
      break;
    }
    default:
      throw std::runtime_error(__FILE__ ": invalid state!");
    }
    break;
  }
  case SDL_EVENT_MOUSE_WHEEL:
  {
    // auto [mx, my] = m_mouse_pos;

    // if not focused, ignore
    if (m_state != State::FOCUSED)
      break;
    // TODO: check mouse position and zoom there
    // event.wheel.mouse_x
    if (event.wheel.integer_y > 0)
    {
      m_zoom_factor *= 0.9;
      if (m_zoom_factor < m_min_zoom)
        m_zoom_factor = m_min_zoom;
    }
    else
    {
      m_zoom_factor *= 1.1;
      if (m_zoom_factor > 1.0)
        m_zoom_factor = 1.0;
    }

    std::pair<float, float> prevDims = {m_render_rect.w, m_render_rect.h};

    m_render_rect.w = m_texture->w * m_zoom_factor;
    m_render_rect.h = m_texture->h * m_zoom_factor;

    // calculate by how much the render rectangle has shrunk/grown
    float x_diff = prevDims.first - m_render_rect.w;
    float y_diff = prevDims.second - m_render_rect.h;

    // now scale by mouse position
    float m_rel_x = m_mouse_pos.first / m_texture_space.w;
    float m_rel_y = m_mouse_pos.second / m_texture_space.h;

    m_render_rect.x += x_diff * m_rel_x;
    m_render_rect.y += y_diff * m_rel_y;

    clampRenderRect();

    handled = UI::EventResult::HANDLED;
    break;
  }
  default:
    break;
  }
  return {};
}

void ZoomableTextureRect::setTexture(SDL_Renderer *renderer,
                                     SDL_Surface *surface,
                                     SDL_ScaleMode scaleMode, bool resetZoom)
{
  if (m_texture)
    SDL_DestroyTexture(m_texture);

  SDL_DestroySurface(m_surface);

  m_surface = surface;

  m_texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_SetTextureScaleMode(m_texture, scaleMode);

  if (resetZoom)
  {
    // initialize render rectangle to full texture
    m_render_rect = {
        .x = 0, .y = 0, .w = (float)m_texture->w, .h = (float)m_texture->h};

    // initialize zoom factor to 1
    m_zoom_factor = 1.f;
    m_min_zoom = 1.f / (m_texture->w / 10.f);
  }

  HandleResizeEvent(m_available_space);
}

void ZoomableTextureRect::clampRenderRect()
{
  if (m_render_rect.x < 0)
  {
    m_render_rect.x = 0;
  }
  else if ((m_render_rect.x + m_render_rect.w) > m_texture->w)
  {
    m_render_rect.x = m_texture->w - m_render_rect.w;
  }

  if (m_render_rect.y < 0)
  {
    m_render_rect.y = 0;
  }
  else if ((m_render_rect.y + m_render_rect.h) > m_texture->h)
  {
    m_render_rect.y = m_texture->h - m_render_rect.h;
  }
}
