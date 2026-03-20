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
  TextBox(float x, float y, const std::string &text, float fontsize = 12);

  ~TextBox() override;

  void setText(const std::string &text);

  void draw(SDL_Renderer *renderer) override;

  bool HandleMouseEvent(SDL_Event &event) override { return false; }

  void HandleResizeEvent(const SDL_FRect &space) override;

private:
  float m_x, m_y;

  std::string m_string;

  TTF_Text *m_text;
};
} // namespace UI
