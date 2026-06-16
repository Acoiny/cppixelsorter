#pragma once

#include <cstdint>
// #include <array>
// #include <string>

#include "sortTask.hpp"

/**
 * Doesn't own the pixel data!
 */
class BaseImageSorter
{
public:
  BaseImageSorter(const SortTask &task);

  // virtual void sort_vertical(int min_hue, int max_hue) = 0;
  //
  // virtual void write_to_file(const std::string &filename) = 0;
  ImageData RunTask(bool reverse);

  ~BaseImageSorter() = default;

protected: // methods
  void sort_vertical_ttb(ImageData &image, int min_hue, int max_hue,
                         bool reverse = false);

  void sort_column_ttb(ImageData &image, int column_index, int start, int end,
                       std::array<int, 360> &brights, bool reverse = false);

  void sort_horizontal_ltr(ImageData &image, int min_hue, int max_hue,
                           bool reverse = false);

  void sort_row_ltr(ImageData &image, int row_index, int start, int end,
                    std::array<int, 360> &brights, bool reverse = false);

  /**
   * Returns the hue of a rgb value
   */
  static int get_hue(uint8_t r, uint8_t g, uint8_t b);

  /**
   * Returns the brightness (0-255) of a rgb value
   */
  static inline int get_brightness(uint8_t r, uint8_t g, uint8_t b)
  {
    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
  }

protected:
  const SortTask &m_task;
};
