#include "Ui/button.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <print>

using namespace UI;

Button::Button(float x, float y, float w, float h) : m_rect{x, y, w, h} {}

void Button::draw(SDL_Renderer *renderer)
{
  SDL_Color col{};
  switch (m_state)
  {
  case ButtonState::IDLE:
    col = m_idleColor;
    break;
  case ButtonState::MOUSE_HOVER:
    col = m_hoverColor;
    break;
  case ButtonState::MOUSE_HELD:
    col = m_mouseDownColor;
    break;
  default:
    break;
  }

  auto [r, g, b, a] = col;
  SDL_SetRenderDrawColor(renderer, r, g, b, a);
  SDL_RenderFillRect(renderer, &m_rect);
}

bool Button::HandleEvent(SDL_Event &event)
{
  bool handled = false;
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
      handled = true;
    }
    // mouse is not over button
    else if (!intersects && m_state == ButtonState::MOUSE_HOVER)
    {
      m_state = ButtonState::IDLE;
      handled = true;
    }
    break;
  }
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
  {
    if (m_state == ButtonState::MOUSE_HOVER)
    {
      m_state = ButtonState::MOUSE_HELD;
      handled = true;
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
      break;
      handled = true;
    }
    break;
  }
  }

  return handled;
}
