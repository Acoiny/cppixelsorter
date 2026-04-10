#pragma once

#include <SDL3/SDL.h>

namespace UI
{
struct Margin
{
  float top = 0;
  float right = 0;
  float bottom = 0;
  float left = 0;
};

class BaseElement
{
public:
  virtual ~BaseElement() = default;

  virtual void draw(SDL_Renderer *renderer) = 0;

  virtual bool HandleMouseEvent(SDL_Event &event) = 0;

  /**
   * Handles resizing when a resize event occurs.
   * @param space the rectangular region the element may occupy
   */
  virtual void HandleResizeEvent(const SDL_FRect &space) = 0;

  void SetMargin(Margin m) { m_margin = m; }
  void SetMargin(float m) { m_margin = {m, m, m, m}; }

protected:
  /**
   * Pads the given rectangle with the element's margin
   */
  inline void applyMargin(SDL_FRect &rect)
  {
    rect.x += m_margin.left;
    rect.w -= m_margin.left + m_margin.right;

    rect.y += m_margin.top;
    rect.h -= m_margin.top + m_margin.bottom;
  }

  Margin m_margin;
};
} // namespace UI
