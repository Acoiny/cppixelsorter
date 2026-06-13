#pragma once

#include "Ui/Colors.hpp"
#include "Ui/baseElement.hpp"
#include "Ui/basic/textButton.hpp"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <vector>

namespace UI
{
class Dropdown : public BaseElement
{
public:
  Dropdown();

  ~Dropdown() override;

  void draw(SDL_Renderer *renderer) override;

  std::pair<EventResult, std::optional<std::shared_ptr<BaseElement>>>
  HandleMouseEvent(SDL_Event &event) override;

  void HandleResizeEvent(const SDL_FRect &space) override;

  /**
   * Adds a new option to the dropdown menu
   */
  Dropdown &AddOption(const std::string &name);

  std::function<void(const std::string &)> onSelectionChange;

private:
  inline bool isIntersecting(float x, float y, SDL_FRect rect)
  {
    return x >= rect.x && x <= (rect.x + rect.w) && // x intersects
           y >= rect.y && y <= (rect.y + rect.h);
  }

private:
  std::shared_ptr<TextButton> m_main_button;
  std::vector<std::shared_ptr<TextButton>> m_options;

  bool m_expanded = false;

  SDL_FRect m_rect;

  /**
   * The space the dropdown menu takes when it is expanded
   */
  SDL_FRect m_expanded_space;
};
}; // namespace UI
