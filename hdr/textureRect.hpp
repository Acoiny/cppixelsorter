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

private:
  SDL_Texture *m_texture = nullptr;

  SDL_FRect m_rect = {0};
};
