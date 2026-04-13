#pragma once

#include "Ui/baseElement.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace UI
{
class IBox : public BaseElement
{
  using ColumnElement = std::pair<std::shared_ptr<BaseElement>, int>;

public:
  virtual ~IBox() = default;

  void draw(SDL_Renderer *renderer) override;
  bool HandleMouseEvent(SDL_Event &event) override;

  void HandleResizeEvent(const SDL_FRect &space) override = 0;

  template <typename T, typename... Args>
  typename std::enable_if<std::is_base_of<BaseElement, T>::value,
                          std::shared_ptr<T>>::type
  addElement(Args &&...args)
  {
    auto el = std::make_shared<T>(std::forward<Args>(args)...);
    m_elements.emplace_back(ColumnElement(el, 1));
    updateDivisor();
    return el;
  }

  template <typename T, typename... Args>
  typename std::enable_if<std::is_base_of<BaseElement, T>::value,
                          std::shared_ptr<T>>::type
  addElementFrac(int frac, Args &&...args)
  {
    auto el = std::make_shared<T>(std::forward<Args>(args)...);
    m_elements.emplace_back(ColumnElement(el, frac));
    updateDivisor();
    return el;
  }

private:
  void updateDivisor();

protected:
  std::vector<ColumnElement> m_elements;

  // the elements contain a size (their "fraction")
  // this is the sum of all fractions. to determine an
  // element's percentage, we just divide it's fraction
  // by this number
  float m_divisor;
};
} // namespace UI
