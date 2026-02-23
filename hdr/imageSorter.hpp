#pragma once

#include <cstdint>
#include <string>

class ImageSorter
{
public:
  ImageSorter(const std::string &filename);

  void sort_vertical(int hue_value);

  void write_to_file(const std::string &filename);

  ~ImageSorter();

private: // methods
  void sort_column(int column_index, int start, int end,
                   std::array<int, 360> &hues);

private:
  uint8_t *m_image;
  int m_width, m_height;
  int m_channels;

  const std::string m_filename;
};
