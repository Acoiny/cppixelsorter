#include "Ui/UiManager.hpp"
#include "Ui/textManager.hpp"
#include <SDL3/SDL.h>

#include <print>

using namespace UI;

UiManager::UiManager(SDL_Renderer *renderer)
{
  TextManager::Init(renderer);

  if (!SDL_GetRenderLogicalPresentation(renderer, &m_letterboxmode.w,
                                        &m_letterboxmode.h,
                                        &m_letterboxmode.mode))
  {
    std::println(stderr, "Unable to get render logical presentation!");
  }
}

UiManager::~UiManager()
{
  m_elements.clear();
  TextManager::Deinit();
}

void UiManager::handleEvent(SDL_Event &event)
{
  if (event.type == SDL_EVENT_WINDOW_RESIZED)
  {
    // TODO: handle resize
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
      event.type == SDL_EVENT_MOUSE_BUTTON_UP)
  {
    for (auto &btn : m_elements)
    {
      // stop iterating, if event has been handled
      if (btn->HandleMouseEvent(event))
        return;
    }
  }
}

void UiManager::draw(SDL_Renderer *renderer)
{
  for (auto &btn : m_elements)
  {
    btn->draw(renderer);
  }
}

bool UiManager::isEventRelevant(SDL_Event &event)
{
  switch (event.type)
  {
  case SDL_EVENT_MOUSE_MOTION:
  case SDL_EVENT_WINDOW_RESIZED:
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
  case SDL_EVENT_MOUSE_BUTTON_UP:
    return true;
  default:
    return false;
  }
}
