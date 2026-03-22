#include "Ui/textBox.hpp"
#include "Ui/textManager.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Ui/logger.hpp"

using namespace UI;

TextBox::TextBox(float x, float y, const std::string &text, float fontsize)
    : m_x(x), m_y(y), m_string(text),
      m_text(TextManager::getInstance().createText(text))
{
}

TextBox::~TextBox() { TTF_DestroyText(m_text); }

void TextBox::setText(const std::string &text)
{
  if (!TTF_SetTextString(m_text, text.c_str(), 0))
  {
    Logger::Error("Error setting text: {}", SDL_GetError());
  }
  else
  {
    m_string = text;
  }
}

void TextBox::draw(SDL_Renderer *renderer)
{
  TTF_DrawRendererText(m_text, m_x, m_y);
}

void TextBox::HandleResizeEvent(const SDL_FRect &space)
{
  m_x = space.x + space.w / 2;
  m_y = space.y + space.h / 2;

  m_y -= TextManager::FONT_SIZE / 2.f;
  m_x -= TextManager::FONT_SIZE * (m_string.size() / 2.f) / 2.f;
}
