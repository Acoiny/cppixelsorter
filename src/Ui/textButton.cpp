#include "Ui/textButton.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Ui/textManager.hpp"

using namespace UI;

TextButton::TextButton(const std::string &text) : TextButton(0, 0, text) {}

TextButton::TextButton(float x, float y, const std::string &text)
    : TextButton(x, y, 60, 20, text)
{
}

TextButton::TextButton(float x, float y, float w, float h,
                       const std::string &text)
    : Button(x, y, w, h), m_text(TextManager::getInstance().createText(text)),
      m_data(text)
{
}

TextButton::~TextButton() { TTF_DestroyText(m_text); }

void TextButton::draw(SDL_Renderer *renderer)
{
  Button::draw(renderer);
  // TODO: center text

  float x_offset =
      m_rect.w / 2.f - TextManager::FONT_SIZE * (m_data.size() / 2.f) / 2.f;
  float y_offset = m_rect.h / 2.f - TextManager::FONT_SIZE / 2.f;

  auto [r, g, b, a] = m_font_color;

  TTF_SetTextColor(m_text, r, g, b, a);
  TTF_DrawRendererText(m_text, m_rect.x + x_offset, m_rect.y + y_offset);
}

void TextButton::setText(const std::string &text)
{
  TTF_SetTextString(m_text, text.c_str(), text.size());
  m_data = text;
}
