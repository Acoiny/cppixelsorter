#pragma once

#include "Ui/baseElement.hpp"
#include <utility>

class TextureRect : public UI::BaseElement
{
public:
  TextureRect() = default;
  ~TextureRect() override;

  void draw(SDL_Renderer *renderer) override;

  bool HandleMouseEvent(SDL_Event &event) override;

  void HandleResizeEvent(const SDL_FRect &space) override;

  void setTexture(SDL_Renderer *renderer, SDL_Surface *surface,
                  SDL_ScaleMode scaleMode = SDL_SCALEMODE_NEAREST);

  TextureRect &setKeepRatio(bool value)
  {
    m_keep_ratio = value;
    return *this;
  }

private:
  SDL_Texture *m_texture = nullptr;
  SDL_Surface *m_surface = nullptr;

  // if false, the image get's streched/squished
  bool m_keep_ratio = true;

  // indicates, if the mouse is over this element
  bool m_focused = false;
  // mouse position from 0.0 to 1.0 relative to the image
  std::pair<float, float> m_relative_mouse_pos = {0, 0};

  SDL_FRect m_texture_space = {0};
  SDL_FRect m_available_space = {0};

  /**
   * Rectangle of the image that should be rendered (zoom)
   */
  SDL_FRect m_render_rect = {0};
  float m_zoom_factor = 1.f;
};
