#pragma once

#include "Ui/baseElement.hpp"
#include <SDL3/SDL_rect.h>
#include <functional>

namespace UI
{
class CheckBox : public BaseElement
{
public:
  CheckBox(bool value = false);

  virtual ~CheckBox() = default;

  void draw(SDL_Renderer *renderer) override;

  std::pair<EventResult, std::optional<std::shared_ptr<BaseElement>>>
  HandleMouseEvent(SDL_Event &event) override;

  void HandleResizeEvent(const SDL_FRect &space) override;

  std::function<void(bool)> onChange;

private:
  inline bool isIntersecting(float x, float y)
  {
    return x >= m_rect.x && x <= (m_rect.x + m_rect.w) && // x intersects
           y >= m_rect.y && y <= (m_rect.y + m_rect.h);
  }

private:
  bool m_value;

  SDL_FRect m_rect;

  SDL_Color m_color_checked = {0, 255, 0, 255};
  SDL_Color m_color_unchecked = {255, 0, 0, 255};
};
} // namespace UI
