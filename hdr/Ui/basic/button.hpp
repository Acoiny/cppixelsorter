#pragma once

#include "Ui/Colors.hpp"
#include "Ui/baseElement.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
#include <functional>

namespace UI
{
/**
 * State indicating the last state of the button
 */
enum class ButtonState
{
  IDLE,
  MOUSE_HOVER,
  MOUSE_HELD,
  // MOUSE_RELEASED,
};

/**
 * Base class for Buttons
 */
class Button : public BaseElement
{
public:
  Button(float x, float y, float w = 60, float h = 20);
  virtual ~Button() override = default;

  void draw(SDL_Renderer *renderer) override;
  std::pair<EventResult, std::optional<std::shared_ptr<BaseElement>>>
  HandleMouseEvent(SDL_Event &event) override;
  void HandleResizeEvent(const SDL_FRect &space) override;

  /**
   * Gets called on left click
   */
  std::function<void()> onLeftClick;

  /**
   * Gets called, when the mouse moves above this button
   */
  std::function<void()> onMouseEnter;

  /**
   * Gets called on right click
   */
  std::function<void()> onRightClick;

  Button &setIdleColor(SDL_Color color)
  {
    m_idleColor = color;
    return *this;
  }
  Button &setHoverColor(SDL_Color color)
  {
    m_hoverColor = color;
    return *this;
  }
  Button &setMouseDownColor(SDL_Color color)
  {
    m_mouseDownColor = color;
    return *this;
  }

protected:
private:
  inline bool isIntersecting(float x, float y)
  {
    return x >= m_rect.x && x <= (m_rect.x + m_rect.w) && // x intersects
           y >= m_rect.y && y <= (m_rect.y + m_rect.h);
  }

protected:
  SDL_Color m_idleColor = UI::Color::BUTTON_BACKGROUND;
  SDL_Color m_hoverColor = UI::Color::BUTTON_HOVER;
  SDL_Color m_mouseDownColor = UI::Color::BUTTON_DOWN;

  SDL_FRect m_rect;

  ButtonState m_state = ButtonState::IDLE;
};
} // namespace UI
