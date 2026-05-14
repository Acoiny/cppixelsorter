#include "Ui/UiManager.hpp"
#include "Ui/textManager.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>

using namespace UI;

UiManager::UiManager(SDL_Renderer *renderer) { TextManager::Init(renderer); }

UiManager::~UiManager()
{
  m_elements.clear();
  TextManager::Deinit();
}

void UiManager::handleEvent(SDL_Event &event)
{
  if (event.type == SDL_EVENT_WINDOW_RESIZED)
  {
    auto x = event.window.data1;
    auto y = event.window.data2;

    for (auto &el : m_elements)
    {
      el->HandleResizeEvent({.x = 0,
                             .y = 0,
                             .w = static_cast<float>(x),
                             .h = static_cast<float>(y)});
    }
    return;
  }

  // handle mouse events
  if (event.type == SDL_EVENT_MOUSE_MOTION ||
      event.type == SDL_EVENT_WINDOW_RESIZED ||
      event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
      event.type == SDL_EVENT_MOUSE_BUTTON_UP ||
      event.type == SDL_EVENT_MOUSE_WHEEL)
  {
    if (m_focused.has_value())
    {
      auto [handled, focused] = m_focused->get()->HandleMouseEvent(event);

      switch (handled)
      {
      case UI::EventResult::UNHANDLED:
        break;
      case UI::EventResult::HANDLED:
        return; // quit trying to handle event
      case UI::EventResult::HANDLED_UPDATE_FOCUS:
        // update focus and then stop
        UI::Logger::Debug("Updated focus, {}", m_focused == focused
                                                   ? "same element"
                                                   : "new element");
        m_focused = focused;
        return;
      }
    }

    for (auto &btn : m_elements)
    {
      // stop iterating, if event has been handled
      auto [handled, focused] = btn->HandleMouseEvent(event);

      switch (handled)
      {
      case UI::EventResult::UNHANDLED:
        break;
      case UI::EventResult::HANDLED:
        return; // quit trying to handle event
      case UI::EventResult::HANDLED_UPDATE_FOCUS:
        // update focus and then stop
        UI::Logger::Debug("Updated focus, {}", m_focused == focused
                                                   ? "same element"
                                                   : "new element");
        m_focused = focused;
        return;
      }
    }
  }

  void UiManager::draw(SDL_Renderer * renderer)
  {
    for (auto &btn : m_elements)
    {
      btn->draw(renderer);
    }

    // draw the focused element on top
    if (m_focused.has_value())
      m_focused->get()->draw(renderer);
  }

  bool UiManager::isEventRelevant(SDL_Event & event)
  {
    switch (event.type)
    {
    case SDL_EVENT_MOUSE_WHEEL:
    case SDL_EVENT_MOUSE_MOTION:
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
      return true;
    default:
      return false;
    }
  }
