#include "Ui/textManager.hpp"
#include "Ui/logger.hpp"
#include "embed_data.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <cassert>
#include <filesystem>

using namespace UI;

TextManager *TextManager::m_instance = nullptr;

TextManager::TextManager(SDL_Renderer *renderer)
{
  m_engine = TTF_CreateRendererTextEngine(renderer);
  // currently only using the default font
  m_font = loadBuiltinFont(); // getDefaultFont();
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

TTF_Font *TextManager::getDefaultFont() const
{
  namespace fs = std::filesystem;

  const char *defaultFonts[] = {
      "/usr/share/fonts/truetype/DejaVuSans.ttf",
      "/usr/share/fonts/open-sans/OpenSans-Regular.ttf",
  };

  for (auto f : defaultFonts)
  {
    if (fs::exists(f))
    {
      TTF_Font *font = TTF_OpenFont(f, FONT_SIZE);
      if (!font)
      {
        Logger::Error("Error trying to load font \"{}\": {}", f,
                      SDL_GetError());
        // just continue iterating
      }
      else
      {
        Logger::Debug("Loaded font {}", f);
      }
    }
  }

  return loadBuiltinFont();
}

TTF_Font *TextManager::loadBuiltinFont() const
{
  Logger::Debug("Loading builtin font");

  TTF_Font *font =
      TTF_OpenFontIO(SDL_IOFromConstMem(assets_Archivo_Regular_ttf,
                                        assets_Archivo_Regular_ttf_len),
                     true, FONT_SIZE);

  if (!font)
  {
    // if we can't even load that... what can we do?
    Logger::Fatal("Unable to load builtin font: {}", SDL_GetError());
  }
  return font;
}
