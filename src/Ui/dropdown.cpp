#include "Ui/dropdown.hpp"
#include <SDL3/SDL_render.h>

using namespace UI;

Dropdown::Dropdown(const std::initializer_list<std::string> &args)
    : m_options(std::move(args))
{
}

Dropdown::~Dropdown() {}

void Dropdown::draw(SDL_Renderer *renderer)
{
  SDL_RenderRect(renderer, &m_rect);
}

bool Dropdown::HandleMouseEvent(SDL_Event &event) { return false; }

void Dropdown::HandleResizeEvent(const SDL_FRect &space)
{
  m_rect = space;
  applyMargin(m_rect);
}
