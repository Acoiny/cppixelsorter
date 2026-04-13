#pragma once

#include "Ui/baseElement.hpp"

class TextureRect : public UI::BaseElement
{
public:
  TextureRect() = default;
  ~TextureRect() override;

  void draw(SDL_Renderer *renderer) override;

  bool HandleMouseEvent(SDL_Event &event) override { return false; }

  void HandleResizeEvent(const SDL_FRect &space) override;

  void setTexture(SDL_Renderer *renderer, SDL_Surface *surface);

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

  SDL_FRect m_texture_space = {0};
  SDL_FRect m_available_space = {0};
};
