#pragma once

#include "filepicker.hpp"
#include "imageData.hpp"
#include "sortTask.hpp"
#include <SDL3/SDL.h>
#include <condition_variable>
#include <memory>
#include <string>
#include <sui/UiManager.hpp>
#include <sui/basic/textBox.hpp>
#include <sui/dropdown.hpp>
#include <sui/zoomableTextureRect.hpp>
#include <thread>

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

  // std::string m_currentFile;

  SDL_Window *m_window = nullptr;
  SDL_Renderer *m_renderer = nullptr;

  // SDL_Surface *m_sorted = nullptr;
  // SDL_Surface *m_original = nullptr;
  ImageData m_original_image;
  ImageData m_sorted_image;

  Filepicker m_filepicker;

  bool m_isRunning = false;

  bool m_autosort = false;

  bool m_reverse_sorting = false;

  struct
  {
    int min = 0;
    int max = 360;
  } m_slider_value;

  SORT_DIRECTION m_sort_direction = SORT_DIRECTION::UP_DOWN;

  std::unique_ptr<sui::UiManager> m_uiManager;

  // textboxes that change frequently
  std::shared_ptr<sui::TextBox> m_infoText;
  // std::shared_ptr<sui::TextBox> m_sliderText;

  std::shared_ptr<sui::Dropdown> m_dropdown;

  std::shared_ptr<sui::ZoomableTextureRect> m_texturerect;

  std::jthread m_thread;
};
