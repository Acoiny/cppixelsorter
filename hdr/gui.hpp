#pragma once

#include "Ui/baseElement.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include <string>
#include <vector>

class Gui
{
public:
  Gui(int width, int height, const std::string &title);

  ~Gui();

  inline bool ShouldClose() { return !m_isRunning; }

  void Update();

  void LoadImage(const std::string &path);

private:
  SDL_FRect get_image_ratio_rect(int image_width, int image_height,
                                 float empty_space_percent = 0);

private:
  SDL_Window *m_window = nullptr;
  SDL_Renderer *m_renderer = nullptr;

  SDL_Surface *m_surface = nullptr;
  SDL_Texture *m_texture = nullptr;

  bool m_isRunning = false;

  std::vector<std::shared_ptr<UI::BaseElement>> m_uiElements;
};
