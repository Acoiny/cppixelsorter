#pragma once

#include "Ui/UiManager.hpp"
#include "Ui/textBox.hpp"
#include "filepicker.hpp"
#include "imageData.hpp"
#include "sortTask.hpp"
#include "textureRect.hpp"
#include <SDL3/SDL.h>
#include <condition_variable>
#include <memory>
#include <string>
#include <thread>

class Gui
{
  friend SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event);

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
  // void UnloadImage();

  void RunSort();

  void ThreadedSort();

  void SaveImage(const std::string &filename);
  void SliderChanged(int value);

private:
  enum class State
  {
    IDLE,
    RUNNING,
    DONE,
  };
  struct
  {
    State state = State::IDLE;
    ImageData result_image;

    std::unique_ptr<SortTask> task;

    std::mutex mutex;
    std::condition_variable cv;
  } m_thread_data;

  std::string m_currentFile;

  SDL_Window *m_window = nullptr;
  SDL_Renderer *m_renderer = nullptr;

  // SDL_Surface *m_sorted = nullptr;
  // SDL_Surface *m_original = nullptr;
  ImageData m_original_image;
  ImageData m_sorted_image;

  Filepicker m_filepicker;

  bool m_isRunning = false;

  bool m_autosort = false;

  struct
  {
    int min = 0;
    int max = 360;
  } m_slider_value;

  std::unique_ptr<UI::UiManager> m_uiManager;

  // textboxes that change frequently
  std::shared_ptr<UI::TextBox> m_infoText;
  // std::shared_ptr<UI::TextBox> m_sliderText;

  std::shared_ptr<TextureRect> m_texturerect;

  std::jthread m_thread;
};
