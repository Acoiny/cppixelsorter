#include "Ui/textButton.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdlib>

#include "Ui/textManager.hpp"

using namespace UI;

TextButton::TextButton(float x, float y, const std::string &text)
    : TextButton(x, y, 60, 20, text)
{
}

TextButton::TextButton(float x, float y, float w, float h,
                       const std::string &text)
    : Button(x, y, w, h), m_text(TextManager::getInstance().createText(text)),
      m_data(text)
{
  const char txt[] = "Button";
}

TextButton::~TextButton() { TTF_DestroyText(m_text); }

void TextButton::draw(SDL_Renderer *renderer)
{
  Button::draw(renderer);
  // TODO: center text

  float mid_x = TextManager::FONT_SIZE * m_data.size();
  auto [x_o, y_o] = calcMiddleOffset();

  auto [r, g, b, a] = m_font_color;

  TTF_SetTextColor(m_text, r, g, b, a);
  TTF_DrawRendererText(m_text, m_rect.x + x_o, m_rect.y + y_o);
}

void TextButton::setText(const std::string &text)
{
  TTF_SetTextString(m_text, text.c_str(), text.size());
  m_data = text;
}

std::tuple<float, float> TextButton::calcMiddleOffset()
{
  float t_x = (TextManager::FONT_SIZE * m_data.size()) / 2.f;
  float t_y = TextManager::FONT_SIZE / 2.f;

  float b_x = m_rect.w / 2;
  float b_y = m_rect.h / 2;

  return {std::abs(t_x - b_x) / 2, std::abs(t_y - b_y) / 2};
}
