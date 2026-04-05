#include "Sorters/verticalSorter.hpp"
#include "Sorters/baseImageSorter.hpp"
#include "imageData.hpp"
#include <cstdint>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <format>
#include <stdexcept>

VerticalSorter::VerticalSorter(const std::string &filename)
    // : m_image(stbi_load(filename.c_str(), &m_width, &m_height, &m_channels,
    //                     CHANNELS)),
    : BaseImageSorter(filename), m_filename(filename)
{
  if (!m_image.pixels)
  {
    throw std::runtime_error(
        std::format("Unable to load image: {}", stbi_failure_reason()));
  }
}

VerticalSorter::VerticalSorter(SDL_Surface *surface, int n_channels)
    : BaseImageSorter(ImageData(surface, n_channels))
{
}

VerticalSorter::~VerticalSorter() { m_image.free(); }

void VerticalSorter::sort_column(int column_index, int start, int end,
                                 std::array<int, 360> &brights)
{
  std::vector<std::array<uint8_t, 3>> sorted_pixels;

  for (std::size_t i = 1; i < brights.size(); i++)
  {
    brights[i] += brights[i - 1];
  }

  sorted_pixels.resize((end - start), {0});

  for (int i = start; i < end; i++)
  {
    uint8_t *pixel = m_image.pixels + (i * m_image.width + column_index) *
                                          m_image.channels; // 3 channels
    uint8_t r = pixel[0], g = pixel[1], b = pixel[2];

    int brightness = get_brightness(r, g, b);

    brights[brightness]--;
    sorted_pixels[brights[brightness]] = {r, g, b};
  }

  // write pixels back
  for (std::size_t i = 0; i < sorted_pixels.size(); i += 1)
  {
    int pixel_height = start + i;
    uint8_t *pixel =
        m_image.pixels + (pixel_height * m_image.width + column_index) *
                             m_image.channels; // 3 channels

    pixel[0] = sorted_pixels[i][0];
    pixel[1] = sorted_pixels[i][1];
    pixel[2] = sorted_pixels[i][2];
  }
}

void VerticalSorter::sort_vertical_ttb(int min_hue, int max_hue)
{
// paralellizing the for loop :)
#pragma omp parallel for
  for (int w = 0; w < m_image.width; w++)
  {
    std::array<int, 360> brightnesses = {0};

    int path_start = -1;

    for (int h = 0; h < m_image.height; h++)
    {
      uint8_t *pixel = m_image.pixels +
                       (h * m_image.width + w) * m_image.channels; // 3 channels
      uint8_t r = pixel[0], g = pixel[1], b = pixel[2];

      int hue = get_hue(r, g, b);

      if (hue >= min_hue && hue <= max_hue)
      {
        // if NOT in path
        if (path_start == -1)
        {
          // start path
          path_start = h;
        }
        brightnesses[get_brightness(r, g, b)]++;
      }
      // pixel is NOT valid anymore
      else
      {
        // we have a path!
        if (path_start != -1)
        {
          int path_end = h;

          sort_column(w, path_start, path_end, brightnesses);
          brightnesses.fill(0);

          path_start = -1;
        }
      }
    }
    if (path_start != -1)
    {
      // we have a path!
      int path_end = m_image.height;

      sort_column(w, path_start, path_end, brightnesses);

      path_start = -1;
    }
  }
}

void VerticalSorter::write_to_file(const std::string &filename)
{
  std::string outfile_name = filename;
  // if filename is empty, it will use the file ending of the original file
  if (filename.size() == 0)
  {
    auto dot_pos = m_filename.find_last_of('.');

    if (dot_pos == std::string::npos)
    {
      throw std::runtime_error(
          std::format("Unable to detect file ending of {}", m_filename));
    }

    outfile_name = "output" + m_filename.substr(dot_pos);
  }

  if (!m_image.write_to_file(outfile_name))
  {
    throw std::runtime_error(
        std::format("Unknown file ending: {}", outfile_name));
  }
}
