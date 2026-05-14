#include "Ui/dropdown.hpp"
#include "Ui/baseElement.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

using namespace UI;

Dropdown::~Dropdown() {}

void Dropdown::draw(SDL_Renderer *renderer)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderFillRect(renderer, &m_rect);

  if (m_state == State::FOCUSED)
  {
    for (auto &btn : m_options)
    {
      btn->draw(renderer);
    }
  }
}

std::pair<EventResult, std::optional<std::shared_ptr<BaseElement>>>
Dropdown::HandleMouseEvent(SDL_Event &event)
{
  EventResult handled = EventResult::UNHANDLED;
  std::optional<std::shared_ptr<BaseElement>> focus;

  // if the dropdown is currently in focus we pass all events down to the
  // elements if none of the elements handled it, we handle it ourselves. But
  // not motions, just the click, because it means "unfocus"
  if (m_state == State::FOCUSED)
  {
    for (auto &el : m_options)
    {
      auto [b_handled, b_focused] = el->HandleMouseEvent(event);

      // a button handled it
      if (b_handled != EventResult::UNHANDLED)
      {
        handled = b_handled;
        focus = b_focused;
        break;
      }
    }

    switch (event.type)
    {
    case SDL_EVENT_MOUSE_MOTION:
    {
      // if the mouse moved over this element (or any of it's children)
      // we say we stop the iteration
      float mx = event.motion.x;
      float my = event.motion.y;

      // if the mouse is moved above this element, we stop handling the event
      if (isIntersecting(mx, my, m_expanded_space))
        handled = EventResult::HANDLED;
      break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    {
      // for a button event, we collapse this element
      // but DON'T say it has been handled, as other
      // elements may still handle it
      m_state = State::COLLAPSED;
      break;
    }
    }

    return {handled, focus};
  }
  switch (event.type)
  {
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
  {
    float mx = event.motion.x;
    float my = event.motion.y;

    bool intersects = isIntersecting(mx, my, m_rect);

    if (intersects)
    {
      handled = EventResult::HANDLED_UPDATE_FOCUS;
      m_state = State::FOCUSED;
      focus = this->shared_from_this();
    }

    break;
  }
  default:
    break;
  }

  return {handled, focus};
}

void Dropdown::HandleResizeEvent(const SDL_FRect &space)
{
  m_rect = space;
  applyMargin(m_rect);

  // keep track of the maximal space this element has occupied
  SDL_FRect max_space = m_rect;

  SDL_FRect opt_rect = m_rect;

  for (auto &opt : m_options)
  {
    opt_rect.y += m_rect.h;
    max_space.h += m_rect.h;
    opt->HandleResizeEvent(opt_rect);
  }

  m_expanded_space = max_space;
}

Dropdown &Dropdown::AddOption(const std::string &title,
                              std::optional<std::function<void()>> action)
{
  m_options.emplace_back(std::make_shared<TextButton>(title));
  if (action)
    m_options.back()->onLeftClick = *action;
  return *this;
}
