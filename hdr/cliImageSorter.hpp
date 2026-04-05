#pragma once

#include "baseImageSorter.hpp"

#include <SDL3/SDL.h>
#include <cstdint>
#include <string>

class CliImageSorter : public BaseImageSorter
{
public:
  CliImageSorter(const std::string &filename);

  CliImageSorter(SDL_Surface *surface, int n_channels = 3);

  void sort_vertical(int hue_value);

  void write_to_file(const std::string &filename);

  ~CliImageSorter();

private: // methods
  void sort_column(int column_index, int start, int end,
                   std::array<int, 360> &hues);

private:
  uint8_t *m_image;
  int m_width, m_height;
  int m_channels;

  bool m_destroy_image = true;

  const std::string m_filename;
};
