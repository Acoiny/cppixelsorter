#pragma once

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
  bool HandleMouseEvent(SDL_Event &event) override;
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
  SDL_Color m_idleColor{0x83, 0x3d, 0xb4, 255};
  SDL_Color m_hoverColor{0xa2, 0x55, 0xd9, 255};
  SDL_Color m_mouseDownColor{0xc3, 0x75, 0xf9, 255};

  SDL_FRect m_rect;

  ButtonState m_state = ButtonState::IDLE;
};
} // namespace UI
