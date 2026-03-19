#include "Ui/textBox.hpp"
#include "Ui/textManager.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <print>

using namespace UI;

TextBox::TextBox(float x, float y, const std::string &text, float fontsize)
    : m_x(x), m_y(y), m_text(TextManager::getInstance().createText(text))
{
}

TextBox::~TextBox() { TTF_DestroyText(m_text); }

void TextBox::setText(const std::string &text)
{
  if (!TTF_SetTextString(m_text, text.c_str(), 0))
  {
    std::println(stderr, "Error setting text: {}", SDL_GetError());
  }
}

void TextBox::draw(SDL_Renderer *renderer)
{
  TTF_DrawRendererText(m_text, m_x, m_y);
}
