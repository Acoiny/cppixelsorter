#include "Ui/cursorManager.hpp"
#include <SDL3/SDL_mouse.h>
#include <cassert>

using namespace UI;

CursorManager::CursorManager()
{
  m_cursors[0] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
  m_cursors[1] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
  m_cursors[2] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
  m_cursors[3] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
}

CursorManager::~CursorManager()
{
  for (SDL_Cursor *cs : m_cursors)
  {
    if (cs)
      SDL_DestroyCursor(cs);
  }
}

// singleton lives in here
CursorManager &CursorManager::getInstance()
{
  static CursorManager instance;
  return instance;
}

void CursorManager::setCursorImpl(int index)
{
  SDL_SetCursor(m_cursors[index]);
}

void CursorManager::SetCursor(CursorStyle style)
{
  int idx = (int)style;
  // TODO: maybe assert index? or use std::array?
  getInstance().setCursorImpl(idx);
}
