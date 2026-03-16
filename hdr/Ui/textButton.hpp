#pragma once

#include "Ui/button.hpp"

#include <SDL3_ttf/SDL_ttf.h>
#include <string>

namespace UI
{
class TextButton : public Button
{
public:
  TextButton(float x, float y, const std::string &text);
  TextButton(float x, float y, float w, float h, const std::string &text);
  ~TextButton() override;

  void draw(SDL_Renderer *renderer) override;

  void setText(const std::string &text);

private:
  // TODO: calculate middle method
  std::tuple<float, float> calcMiddleOffset();

private:
  TTF_Text *m_text;

  std::string m_data;

  SDL_Color m_font_color{0, 0, 0, 255};
};
} // namespace UI
