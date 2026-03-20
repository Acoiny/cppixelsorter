#include "Ui/container/ibox.hpp"
#include <numeric>

using namespace UI;

void IBox::draw(SDL_Renderer *renderer)
{
  for (auto el : m_elements)
  {
    el.first->draw(renderer);
  }
}

bool IBox::HandleMouseEvent(SDL_Event &event)
{
  for (auto el : m_elements)
  {
    if (el.first->HandleMouseEvent(event))
      return true;
  }
  return false;
}

void IBox::updateDivisor()
{
  m_divisor = std::accumulate(m_elements.begin(), m_elements.end(), 0,
                              [](float total, const auto a) -> float
                              { return a.second + total; });
}
