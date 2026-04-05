#pragma once

// #include "Sorters/baseImageSorter.hpp"

#include "baseImageSorter.hpp"
#include <SDL3/SDL.h>
#include <string>

class VerticalSorter : public BaseImageSorter
{
public:
  VerticalSorter(const std::string &filename);

  VerticalSorter(SDL_Surface *surface, int n_channels = 3);

  void sort_vertical_ttb(int min_hue, int max_hue = 360);

  void write_to_file(const std::string &filename);

  ~VerticalSorter();

private: // methods
  void sort_column(int column_index, int start, int end,
                   std::array<int, 360> &brights);

  const std::string m_filename;
};
