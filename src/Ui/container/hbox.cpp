#include "Ui/container/hbox.hpp"
#include <SDL3/SDL_rect.h>

using namespace UI;

void HBox::HandleResizeEvent(const SDL_FRect &space)
{
  const float column_size = space.w / m_elements.size();

  float offset = 0;

  for (auto el : m_elements)
  {
    SDL_FRect newSpace = space;
    newSpace.x += offset;
    newSpace.w = column_size;
    offset += column_size;
    el->HandleResizeEvent(newSpace);
  }
}
