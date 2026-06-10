#include "Ui/basic/button.hpp"
#include "Ui/baseElement.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

using namespace UI;

Button::Button(float x, float y, float w, float h) : m_rect{x, y, w, h} {}

void Button::draw(SDL_Renderer *renderer)
{
  SDL_Color col{};

  bool render_outline = false;

  switch (m_state)
  {
  case ButtonState::IDLE:
    col = m_idleColor;
    break;
  case ButtonState::MOUSE_HOVER:
    col = m_hoverColor;
    render_outline = true;
    break;
  case ButtonState::MOUSE_HELD:
    col = m_mouseDownColor;
    render_outline = true;
    break;
  default:
    break;
  }

  auto [r, g, b, a] = col;
  SDL_SetRenderDrawColor(renderer, r, g, b, a);
  SDL_RenderFillRect(renderer, &m_rect);

  if (render_outline)
  {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &m_rect);
  }
}

std::pair<EventResult, std::optional<std::shared_ptr<BaseElement>>>
Button::HandleMouseEvent(SDL_Event &event)
{
  EventResult handled = EventResult::UNHANDLED;
  switch (event.type)
  {
  case SDL_EVENT_MOUSE_MOTION:
  {
    float mx = event.motion.x, my = event.motion.y;
    bool intersects = isIntersecting(mx, my);

    // mouse entered
    if (intersects && m_state == ButtonState::IDLE)
    {
      m_state = ButtonState::MOUSE_HOVER;

      if (onMouseEnter)
        onMouseEnter();
      handled = EventResult::HANDLED;
    }
    // mouse is not over button
    else if (!intersects && m_state == ButtonState::MOUSE_HOVER)
    {
      m_state = ButtonState::IDLE;
      handled = EventResult::HANDLED;
    }
    break;
  }
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
  {
    if (m_state == ButtonState::MOUSE_HOVER)
    {
      m_state = ButtonState::MOUSE_HELD;
      handled = EventResult::HANDLED;
    }
    break;
  }
  case SDL_EVENT_MOUSE_BUTTON_UP:
  {
    if (m_state == ButtonState::MOUSE_HELD)
    {
      float mx = event.motion.x, my = event.motion.y;
      bool intersects = isIntersecting(mx, my);

      if (intersects)
      {
        m_state = ButtonState::MOUSE_HOVER;
        if (onLeftClick)
          onLeftClick();
      }
      else
      {
        m_state = ButtonState::IDLE;
      }
      handled = EventResult::HANDLED;
    }
    break;
  }
  }

  return {handled, {}};
}

void Button::HandleResizeEvent(const SDL_FRect &space)
{
  m_rect = space;
  applyMargin(m_rect);
}
