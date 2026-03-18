#pragma once

#include <chrono>
class Timer
{
public:
  Timer() { restart(); }

  inline Timer &restart()
  {
    m_start_time = std::chrono::high_resolution_clock::now();
    return *this;
  }

  inline std::chrono::duration<double> get()
  {
    return std::chrono::high_resolution_clock::now() - m_start_time;
  }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
};
