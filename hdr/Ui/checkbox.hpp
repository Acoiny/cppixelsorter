#pragma once

#include "Ui/baseElement.hpp"
#include <SDL3/SDL_rect.h>

namespace UI
{
class CheckBox : public BaseElement
{
  static constexpr float VERTICAL_MARGIN = 10;

public:
  CheckBox(bool value = false);

  virtual ~CheckBox() = default;

  void draw(SDL_Renderer *renderer) override;

  bool HandleMouseEvent(SDL_Event &event) override;

  void HandleResizeEvent(const SDL_FRect &space) override;

private:
  inline bool isIntersecting(float x, float y)
  {
    return x >= m_rect.x && x <= (m_rect.x + m_rect.w) && // x intersects
           y >= m_rect.y && y <= (m_rect.y + m_rect.h);
  }

private:
  bool m_value;

  SDL_FRect m_rect;
};
} // namespace UI
