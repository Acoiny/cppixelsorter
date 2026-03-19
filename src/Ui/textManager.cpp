#include "Ui/textManager.hpp"

#include <SDL3_ttf/SDL_ttf.h>

#include <cassert>
#include <filesystem>

#include <iostream>
#include <print>

using namespace UI;

TextManager *TextManager::m_instance = nullptr;

TextManager::TextManager(SDL_Renderer *renderer)
{
  m_engine = TTF_CreateRendererTextEngine(renderer);
  m_font = TTF_OpenFont(getDefaultFont().c_str(), FONT_SIZE);
}

TextManager::~TextManager()
{
  TTF_CloseFont(m_font);
  TTF_DestroyRendererTextEngine(m_engine);
}

TextManager &TextManager::getInstance()
{
  assert(m_instance);
  return *m_instance;
}

void TextManager::Init(SDL_Renderer *renderer)
{
  m_instance = new TextManager(renderer);
}

void TextManager::Deinit()
{
  if (m_instance)
    delete m_instance;
  m_instance = nullptr;
}

TTF_Text *TextManager::createText(const std::string &text)
{
  TTF_Text *txt = TTF_CreateText(m_engine, m_font, text.c_str(), text.size());
  return txt;
}

const std::string TextManager::getDefaultFont() const
{
  namespace fs = std::filesystem;

  const auto defaultFonts = {"/usr/share/fonts/truetype/DejaVuSans.ttf",
                             "/usr/share/fonts/open-sans/OpenSans-Regular.ttf"};

  for (auto f : defaultFonts)
  {
    if (fs::exists(f))
    {
      return f;
    }
  }

  std::println(std::cerr, "None of the set fonts found");

  return "";
}
