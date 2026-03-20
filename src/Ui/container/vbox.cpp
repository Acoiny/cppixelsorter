#include "Ui/container/vbox.hpp"

using namespace UI;

void VBox::HandleResizeEvent(const SDL_FRect &space)
{
  const float row_size = space.h / m_elements.size();

  float offset = 0;

  for (auto el : m_elements)
  {
    SDL_FRect newSpace = space;
    newSpace.y += offset;
    newSpace.h = row_size;
    offset += row_size;
    el->HandleResizeEvent(newSpace);
  }
}
