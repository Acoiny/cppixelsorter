#include "Ui/checkbox.hpp"
#include "Ui/baseElement.hpp"
#include <SDL3/SDL_render.h>

using namespace UI;

CheckBox::CheckBox(bool value) : m_value(value) {}

void CheckBox::draw(SDL_Renderer *renderer)
{
  auto [r, g, b, a] = m_value ? m_color_checked : m_color_unchecked;
  SDL_SetRenderDrawColor(renderer, r, g, b, a);
  SDL_RenderFillRect(renderer, &m_rect);
}

std::pair<EventResult, std::optional<std::shared_ptr<BaseElement>>>
CheckBox::HandleMouseEvent(SDL_Event &event)
{
  EventResult handled = EventResult::UNHANDLED;

  float m_x = event.motion.x;
  float m_y = event.motion.y;

  if (isIntersecting(m_x, m_y))
  {
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
      m_value = !m_value;
      if (onChange)
        onChange(m_value);
    }

    handled = EventResult::HANDLED;
  }
  return {handled, {}};
}

void CheckBox::HandleResizeEvent(const SDL_FRect &space)
{
  m_rect = space;
  applyMargin(m_rect);
}
