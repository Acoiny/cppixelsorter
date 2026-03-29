#include "Ui/checkbox.hpp"

using namespace UI;

CheckBox::CheckBox(bool value) : m_value(value) {}

void CheckBox::draw(SDL_Renderer *renderer) {}

bool CheckBox::HandleMouseEvent(SDL_Event &event)
{
  float m_x = event.motion.x;
  float m_y = event.motion.y;

  if (isIntersecting(m_x, m_y))
  {
  }
}
