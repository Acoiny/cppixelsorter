#pragma once

#include "Ui/button.hpp"

#include <SDL3_ttf/SDL_ttf.h>
#include <string>

namespace UI
{
class TextButton : public Button
{
public:
  TextButton(const std::string &text);
  TextButton(float x, float y, const std::string &text);
  TextButton(float x, float y, float w, float h, const std::string &text);
  ~TextButton() override;

  void draw(SDL_Renderer *renderer) override;

  void setText(const std::string &text);

private:
  TTF_Text *m_text;

  std::string m_data;
};
} // namespace UI
