#include "Ui/dropdown.hpp"
#include "Ui/baseElement.hpp"
#include "Ui/logger.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <memory>
#include <print>

using namespace UI;

Dropdown::Dropdown() : m_main_button(std::make_shared<TextButton>("<empty>"))
{
  m_main_button->onLeftClick = [this]() { m_expanded = !m_expanded; };
}

Dropdown::~Dropdown() {}

void Dropdown::draw(SDL_Renderer *renderer)
{
  m_main_button->draw(renderer);

  if (m_expanded)
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

  {
    auto [m_handled, m_focus] = m_main_button->HandleMouseEvent(event);
    if (m_handled != EventResult::UNHANDLED)
    {
      if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
      {
        m_handled = EventResult::HANDLED_UPDATE_FOCUS;
        m_focus = this->shared_from_this();
      }

      return {m_handled, m_focus};
    }
  }

  // if the dropdown is currently in focus we pass all events down to the
  // elements if none of the elements handled it, we handle it ourselves. But
  // not motions, just the click, because it means "unfocus"
  if (m_expanded)
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

    // none of the buttons have handled the event
    switch (event.type)
    {
    case SDL_EVENT_MOUSE_BUTTON_UP:
    {
      m_expanded = false;
      handled = EventResult::HANDLED_UPDATE_FOCUS;
      break;
    }
    }
  }

  return {handled, focus};
}

void Dropdown::HandleResizeEvent(const SDL_FRect &space)
{
  m_rect = space;
  applyMargin(m_rect);

  m_main_button->HandleResizeEvent(m_rect);

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

Dropdown &Dropdown::AddOption(const std::string &name)
{
  auto ptr = std::make_shared<TextButton>(name);

  // if this is the first option, select it
  if (m_options.size() == 0)
  {
    m_main_button->setText(name);
  }

  ptr->onLeftClick = [this, name]() { m_main_button->setText(name); };
  m_options.push_back(ptr);
  return *this;
}

const std::string &Dropdown::getSelected() const
{
  return m_main_button->getText();
}
