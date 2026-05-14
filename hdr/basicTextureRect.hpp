#pragma once

#include "Ui/baseElement.hpp"
namespace UI
{
class BasicTextureRect : public BaseElement
{
public:
  BasicTextureRect() = default;
  ~BasicTextureRect() override;

  void draw(SDL_Renderer *renderer) override;

  std::pair<EventResult, std::optional<std::shared_ptr<BaseElement>>>
  HandleMouseEvent(SDL_Event &event) override
  {
    return {EventResult::UNHANDLED, {}};
  }

  void HandleResizeEvent(const SDL_FRect &space) override;

  void setTexture(SDL_Renderer *renderer, SDL_Surface *surface,
                  SDL_ScaleMode scaleMode = SDL_SCALEMODE_NEAREST);

  BasicTextureRect &setKeepRatio(bool value)
  {
    m_keep_ratio = value;
    return *this;
  }

protected:
  SDL_Texture *m_texture = nullptr;
  SDL_Surface *m_surface = nullptr;

  // if false, the image get's streched/squished
  bool m_keep_ratio = true;

  SDL_FRect m_texture_space = {0};
  SDL_FRect m_available_space = {0};
};
} // namespace UI
