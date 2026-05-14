#pragma once

#include <SDL3/SDL_rect.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

#include "Ui/baseElement.hpp"

namespace UI
{
class TextBox : public BaseElement
{
public:
  TextBox(const std::string &text, float fontsize = 12);
  TextBox(float x, float y, const std::string &text, float fontsize = 12);

  ~TextBox() override;

  void setText(const std::string &text);

  void draw(SDL_Renderer *renderer) override;

  std::pair<EventResult, std::optional<std::shared_ptr<BaseElement>>>
  HandleMouseEvent(SDL_Event &event) override
  {
    return {EventResult::UNHANDLED, {}};
  }

  void HandleResizeEvent(const SDL_FRect &space) override;

private:
  SDL_FRect m_rect = {0};
  float m_x, m_y;

  std::string m_string;

  TTF_Text *m_text;
};
} // namespace UI
