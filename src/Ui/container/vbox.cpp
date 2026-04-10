#include "Ui/container/vbox.hpp"

using namespace UI;

void VBox::HandleResizeEvent(const SDL_FRect &space)
{
  float offset = 0;

  for (auto el : m_elements)
  {
    const float row_size = ((float)el.second / m_divisor) * space.h;

    SDL_FRect newSpace = space;
    newSpace.y += offset;
    newSpace.h = row_size;
    offset += row_size;
    el.first->HandleResizeEvent(newSpace);
  }
}
