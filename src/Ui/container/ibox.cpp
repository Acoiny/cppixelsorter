#include "Ui/container/ibox.hpp"

using namespace UI;

void IBox::draw(SDL_Renderer *renderer)
{
  for (auto el : m_elements)
  {
    el->draw(renderer);
  }
}

bool IBox::HandleMouseEvent(SDL_Event &event)
{
  for (auto el : m_elements)
  {
    if (el->HandleMouseEvent(event))
      return true;
  }
  return false;
}
