#pragma once

#include "basicTextureRect.hpp"
#include <SDL3/SDL_rect.h>
#include <utility>

class ZoomableTextureRect : public UI::BasicTextureRect
{
  enum class State
  {
    IDLE,     // default
    FOCUSED,  // cursor is over the image
    DRAGGING, // image is being dragged
  };

public:
  ZoomableTextureRect() = default;

  void draw(SDL_Renderer *renderer) override;

  bool HandleMouseEvent(SDL_Event &event) override;

  void setTexture(SDL_Renderer *renderer, SDL_Surface *surface,
                  SDL_ScaleMode scaleMode = SDL_SCALEMODE_NEAREST,
                  bool resetZoom = true);

private:
  /**
   * Clamps the render rectangle that is used for zooming
   * and dragging
   */
  void clampRenderRect();

private:
  // indicates, if the mouse is over this element
  State m_state = State::IDLE;

  // mouse position inside the image
  std::pair<float, float> m_mouse_pos = {0, 0};

  /**
   * Rectangle of the image that should be rendered (zoom)
   */
  SDL_FRect m_render_rect = {0};
  float m_zoom_factor = 1.f;
  float m_min_zoom = 1.f;
};
