#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>
#include <type_traits>
#include <vector>

#include "Ui/baseElement.hpp"

namespace UI
{
class UiManager
{
public:
  UiManager(SDL_Renderer *renderer, const std::string &app_name);
  ~UiManager();

  void handleEvent(SDL_Event &event);

  void draw(SDL_Renderer *renderer);

  bool isEventRelevant(SDL_Event &event);

  template <typename T, typename... Args>
  typename std::enable_if<std::is_base_of<BaseElement, T>::value,
                          std::shared_ptr<T>>::type
  addElement(Args &&...args)
  {
    auto el = std::make_shared<T>(std::forward<Args>(args)...);
    m_elements.emplace_back(el);
    return el;
  }

private:
  std::vector<std::shared_ptr<BaseElement>> m_elements;

  std::optional<std::shared_ptr<BaseElement>> m_focused;
};
} // namespace UI
