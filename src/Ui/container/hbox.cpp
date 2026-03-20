#include "Ui/container/hbox.hpp"
#include <SDL3/SDL_rect.h>

using namespace UI;

void HBox::HandleResizeEvent(const SDL_FRect &space)
{
  float offset = 0;

  for (auto el : m_elements)
  {
    const float column_size = ((float)el.second / m_divisor) * space.w;

    SDL_FRect newSpace = space;
    newSpace.x += offset;
    newSpace.w = column_size;
    offset += column_size;
    el.first->HandleResizeEvent(newSpace);
  }
}
