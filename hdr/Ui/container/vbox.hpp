#pragma once

#include "Ui/container/ibox.hpp"
#include <SDL3/SDL_rect.h>

namespace UI
{
class VBox : public IBox
{
public:
  VBox() = default;
  virtual ~VBox() = default;

  void HandleResizeEvent(const SDL_FRect &space) override;
};
}; // namespace UI
