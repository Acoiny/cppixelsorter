#pragma once

#include "Ui/baseElement.hpp"
#include <initializer_list>
#include <string>
#include <vector>

namespace UI
{
class Dropdown : public BaseElement
{
public:
  Dropdown(const std::initializer_list<std::string> &args = {});

  ~Dropdown() override;

  void draw(SDL_Renderer *renderer) override;

  bool HandleMouseEvent(SDL_Event &event) override;

  /**
   * Handles resizing when a resize event occurs.
   * @param space the rectangular region the element may occupy
   */
  void HandleResizeEvent(const SDL_FRect &space) override;

private:
  std::vector<std::string> m_options;

  SDL_FRect m_rect;
};
}; // namespace UI
