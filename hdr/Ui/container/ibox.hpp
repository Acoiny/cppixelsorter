#pragma once

#include "Ui/baseElement.hpp"
#include <memory>
#include <vector>

namespace UI
{
class IBox : public BaseElement
{
public:
  virtual ~IBox() = default;

  void draw(SDL_Renderer *renderer) override;
  bool HandleMouseEvent(SDL_Event &event) override;

  /**
   * Handles resizing when a resize event occurs.
   * @param space the rectangular region the element may occupy
   */
  void HandleResizeEvent(const SDL_FRect &space) override = 0;

  template <typename T, typename... Args>
  typename std::enable_if<std::is_base_of<BaseElement, T>::value,
                          std::shared_ptr<T>>::type
  addElement(Args &&...args)
  {
    auto el = std::make_shared<T>(std::forward<Args>(args)...);
    m_elements.emplace_back(el);
    return el;
  }

protected:
  std::vector<std::shared_ptr<BaseElement>> m_elements;
};
} // namespace UI
