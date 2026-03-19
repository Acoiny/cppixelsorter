#pragma once

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

  bool HandleEvent(SDL_Event &event) override { return false; }

private:
  float m_x, m_y;

  TTF_Text *m_text;
};
} // namespace UI
