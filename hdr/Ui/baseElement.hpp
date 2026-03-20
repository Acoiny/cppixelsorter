#pragma once

#include <SDL3/SDL_render.h>

namespace UI
{
class BaseElement
{
public:
  virtual ~BaseElement() = 0;

  virtual void draw(SDL_Renderer *renderer) = 0;

  virtual bool HandleMouseEvent(SDL_Event &event) = 0;

  /**
   * Handles resizing when a resize event occurs.
   * @param space the rectangular region the element may occupy
   */
  virtual void HandleResizeEvent(const SDL_FRect &space) = 0;
};
} // namespace UI
