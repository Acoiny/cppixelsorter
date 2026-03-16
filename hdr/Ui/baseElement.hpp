#pragma once

#include <SDL3/SDL_render.h>

namespace UI
{
class BaseElement
{
public:
  virtual ~BaseElement() = default;

  virtual void draw(SDL_Renderer *renderer) = 0;

  virtual bool HandleEvent(SDL_Event &event) = 0;
};
} // namespace UI
