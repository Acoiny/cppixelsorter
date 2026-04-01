#pragma once

#include "Ui/baseElement.hpp"
#include <SDL3/SDL_rect.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <functional>

namespace UI
{
/**
 * Slider UI-Element. Will produce values between min and max.
 * The callback gets called whenever a value change, greater than the set
 * threshold occurs.
 */
template <typename T> class Slider : public BaseElement
{
  // TODO: clean up these magic values!
  static constexpr float MARGIN_SIDE = 10;
  static constexpr float HEIGHT = 5;
  static constexpr float SELECTOR_RADIUS = 5;
  static constexpr T CHANGE_THRESHOLD = 1;
  static constexpr SDL_Color COLOR_IDLE = {0x83, 0x3d, 0xb4, 255};
  static constexpr SDL_Color COLOR_ACTIVE = {0xc3, 0x75, 0xf9, 255};

  enum class State
  {
    IDLE,
    HOVER,
    HELD,
  };

public:
  Slider(T min, T max) : m_min(min), m_max(max), m_value(min), m_lastChange(min)
  {
  }

  void draw(SDL_Renderer *renderer) override
  {
    // drawing bar in black for now
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // DEBUG
    // SDL_RenderRect(renderer, &m_space);
    // END DEBUG
    SDL_RenderFillRect(renderer, &m_bar);

    const auto [x_begin, y_value] = getSelectorCenter();

    SDL_FRect m_selector = {.x = x_begin - SELECTOR_RADIUS,
                            .y = y_value - SELECTOR_RADIUS,
                            .w = SELECTOR_RADIUS * 2,
                            .h = SELECTOR_RADIUS * 2};

    switch (m_state)
    {
    case State::IDLE:
    {
      auto [r, g, b, a] = COLOR_IDLE;
      SDL_SetRenderDrawColor(renderer, r, g, b, a);
      break;
    }
    case State::HOVER:
    case State::HELD:
    {
      auto [r, g, b, a] = COLOR_ACTIVE;
      SDL_SetRenderDrawColor(renderer, r, g, b, a);
      break;
    }
    }
    SDL_RenderFillRect(renderer, &m_selector);
  }

  bool HandleMouseEvent(SDL_Event &event) override
  {
    bool handled = false;

    switch (event.type)
    {
    case SDL_EVENT_MOUSE_MOTION:
    {
      float mx = event.motion.x, my = event.motion.y;
      bool intersects = isIntersecting(mx, my);

      switch (m_state)
      {
      case State::IDLE:
      {
        if (intersects)
        {
          m_state = State::HOVER;
          handled = true;
        }
        break;
      }
      case State::HOVER:
      {
        if (!intersects)
        {
          m_state = State::IDLE;
          handled = true;
        }
        break;
      }
      case State::HELD:
      {
        float mx = event.motion.x;
        // the moving of the selector
        m_value = xToValue(mx);

        if (std::abs(m_value - m_lastChange) >= CHANGE_THRESHOLD)
        {
          // update last change
          m_lastChange = m_value;

          // evoke callback
          if (onValueChange)
            onValueChange(m_value);
        }

        handled = true;
        break;
      }
      }
      break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    {
      if (m_state == State::HOVER)
      {
        m_state = State::HELD;
        handled = true;
      }
      break;
    }
    case SDL_EVENT_MOUSE_BUTTON_UP:
    {
      if (m_state == State::HELD)
      {
        float mx = event.motion.x, my = event.motion.y;
        m_state = isIntersecting(mx, my) ? State::HOVER : State::IDLE;
        handled = true;
      }
      break;
    }
    }

    return handled;
  }

  void HandleResizeEvent(const SDL_FRect &space) override
  {
    m_space = space;
    m_bar = space;

    // margin left and right
    m_bar.x += MARGIN_SIDE;
    m_bar.w -= MARGIN_SIDE * 2;

    // calculating the middle
    m_bar.y += m_bar.h / 2 - HEIGHT / 2;
    m_bar.h = HEIGHT;
  }

  // the callback for value change events
  std::function<void(T)> onValueChange;

private:
  inline bool isIntersecting(float x, float y)
  {
    const auto [sel_x, sel_y] = getSelectorCenter();
    return std::abs(sel_x - x) < (SELECTOR_RADIUS) &&
           std::abs(sel_y - y) < (SELECTOR_RADIUS);
  }

  /**
   * Finds the x-coordinate of the given value
   */
  inline float getXofValue(T value) const
  {
    float min_x = m_bar.x;
    float max_x = min_x + m_bar.w;
    return std::lerp(min_x, max_x,
                     static_cast<float>(value - m_min) /
                         static_cast<float>(m_max - m_min));
  }

  inline T xToValue(float x) const
  {
    float min_x = m_bar.x;
    float max_x = min_x + m_bar.w;
    return std::clamp(
        static_cast<T>(std::lerp(m_min, m_max, (x - min_x) / (max_x - min_x))),
        m_min, m_max);
  }

  /**
   * Returns the centerpoint of the selector
   */
  inline SDL_FPoint getSelectorCenter() const
  {
    // float x_begin = m_bar.x + m_value;
    const float x_value = getXofValue(m_value);
    const float y_value = m_bar.y + m_bar.h / 2;
    return {x_value, y_value};
  }

private:
  const T m_min, m_max;

  T m_value;
  T m_lastChange;
  State m_state = State::IDLE;

  SDL_FRect m_space;
  SDL_FRect m_bar;
};
}; // namespace UI
