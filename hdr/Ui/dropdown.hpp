#pragma once

#include "Ui/Colors.hpp"
#include "Ui/baseElement.hpp"
#include "Ui/textBox.hpp"
#include "Ui/textButton.hpp"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <string>
#include <vector>

namespace UI
{
class Dropdown : public BaseElement
{
  enum class State
  {
    COLLAPSED,
    FOCUSED,
  };

public:
  Dropdown() = default;

  ~Dropdown() override;

  void draw(SDL_Renderer *renderer) override;

  std::pair<EventResult, std::optional<std::shared_ptr<BaseElement>>>
  HandleMouseEvent(SDL_Event &event) override;

  /**
   * Handles resizing when a resize event occurs.
   * @param space the rectangular region the element may occupy
   */
  void HandleResizeEvent(const SDL_FRect &space) override;

  Dropdown &AddOption(const std::string &title,
                      std::optional<std::function<void()>> action = {});

private:
  inline bool isIntersecting(float x, float y, SDL_FRect rect)
  {
    return x >= rect.x && x <= (rect.x + rect.w) && // x intersects
           y >= rect.y && y <= (rect.y + rect.h);
  }

private:
  std::vector<std::shared_ptr<TextButton>> m_options;
  std::shared_ptr<TextBox> m_selected_option;

  State m_state = State::COLLAPSED;

  SDL_FRect m_rect;

  /**
   * The space the dropdown menu takes when it is expanded
   */
  SDL_FRect m_expanded_space;

  SDL_Color m_background_color = Color::BUTTON_BACKGROUND;
};
}; // namespace UI
