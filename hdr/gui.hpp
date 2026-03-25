#pragma once

#include "Ui/UiManager.hpp"
#include "Ui/textBox.hpp"
#include "textureRect.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include <string>

class Gui
{
public:
  Gui(int width, int height, const std::string &title);

  ~Gui();

  inline bool ShouldClose() { return !m_isRunning; }

  void Update();

  void LoadImage(const std::string &path);

private:
  /**
   * Unloads the currently loaded image.
   * Does nothing, if no image is loaded.
   */
  void UnloadImage();

  void PickFile();
  void RunSort();
  void SaveFile();
  void SliderChanged(int value);

private:
  std::string m_currentFile;
  std::shared_ptr<UI::TextBox> m_fileName;

  SDL_Window *m_window = nullptr;
  SDL_Renderer *m_renderer = nullptr;

  SDL_Surface *m_sorted = nullptr;
  SDL_Surface *m_original = nullptr;

  bool m_isRunning = false;

  int m_slider_value = 0;

  std::unique_ptr<UI::UiManager> m_uiManager;

  // textboxes that change frequently
  std::shared_ptr<UI::TextBox> m_infoText;
  std::shared_ptr<UI::TextBox> m_sliderText;

  std::shared_ptr<TextureRect> m_texturerect;
};
