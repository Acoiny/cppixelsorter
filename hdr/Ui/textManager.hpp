#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include <string>

namespace UI
{
class TextManager
{
public:
  static constexpr int FONT_SIZE = 12;

  TTF_Text *createText(const std::string &text);

  static TextManager &getInstance();

  /**
   * Initializes the global textmanager
   */
  static void Init(SDL_Renderer *renderer);

  static void Deinit();

private:
  static TextManager *m_instance;

  TextManager(SDL_Renderer *renderer);
  ~TextManager();

  const std::string getDefaultFont() const;

private:
  TTF_TextEngine *m_engine = nullptr;

  TTF_Font *m_font = nullptr;
};
} // namespace UI
