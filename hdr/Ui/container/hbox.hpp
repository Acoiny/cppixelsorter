#pragma once

#include "Ui/container/ibox.hpp"

namespace UI
{
class HBox : public IBox
{
public:
  HBox() = default;
  virtual ~HBox() = default;

  void HandleResizeEvent(const SDL_FRect &space) override;
};
} // namespace UI
