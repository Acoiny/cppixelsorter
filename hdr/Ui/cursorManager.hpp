#pragma once

#include <SDL3/SDL_mouse.h>
namespace UI
{
class CursorManager
{
private:
  CursorManager();
  ~CursorManager();

  /**
   * Gets the singelton instance
   */
  static CursorManager &getInstance();

  void setCursorImpl(int index);

public:
  enum class CursorStyle
  {
    DEFAULT,
    POINT,
    WAIT,
    MOVE,
  };

  static void SetCursor(CursorStyle style = CursorStyle::DEFAULT);

private:
  SDL_Cursor *m_cursors[5] = {0};
};
} // namespace UI
