#include "Sorters/baseImageSorter.hpp"
#include "imageData.hpp"
#include "sortTask.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <sys/types.h>
#include <utility>
#include <vector>

BaseImageSorter::BaseImageSorter(const SortTask &task) : m_task(task) {}

ImageData BaseImageSorter::RunTask()
{
  ImageData newImage = m_task.image;

  switch (m_task.sort_direction)
  {
  case SORT_DIRECTION::VERTICAL_TTB:
    sort_vertical_ttb(newImage, m_task.hue_values.min, m_task.hue_values.max);
    break;
  case SORT_DIRECTION::VERTICAL_BTT:
    sort_vertical_ttb(newImage, m_task.hue_values.min, m_task.hue_values.max,
                      true);
    break;
  default:
    throw std::runtime_error("Unsupported sort option!");
  }

  return newImage;
}

void BaseImageSorter::sort_vertical_ttb(ImageData &image, int min_hue,
                                        int max_hue, bool reverse)
{
// paralellizing the for loop :)
#pragma omp parallel for
  for (int w = 0; w < image.width; w++)
  {
    std::array<int, 360> brightnesses = {0};

    int path_start = -1;

    for (int h = 0; h < image.height; h++)
    {
      uint8_t *pixel =
          image.pixels + (h * image.width + w) * image.channels; // 3 channels
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

          sort_column_ttb(image, w, path_start, path_end, brightnesses,
                          reverse);
          brightnesses.fill(0);

          path_start = -1;
        }
      }
    }
    if (path_start != -1)
    {
      // we have a path!
      int path_end = image.height;

      sort_column_ttb(image, w, path_start, path_end, brightnesses, reverse);

      path_start = -1;
    }
  }
}

void BaseImageSorter::sort_column_ttb(ImageData &image, int column_index,
                                      int start, int end,
                                      std::array<int, 360> &brights,
                                      bool reverse)
{
  std::vector<std::array<uint8_t, 3>> sorted_pixels;

  for (std::size_t i = 1; i < brights.size(); i++)
  {
    brights[i] += brights[i - 1];
  }

  sorted_pixels.resize((end - start), {0});

  for (int i = start; i < end; i++)
  {
    uint8_t *pixel = image.pixels + (i * image.width + column_index) *
                                        image.channels; // 3 channels
    uint8_t r = pixel[0], g = pixel[1], b = pixel[2];

    int brightness = get_brightness(r, g, b);

    brights[brightness]--;
    sorted_pixels[brights[brightness]] = {r, g, b};
  }

  if (reverse)
    std::reverse(sorted_pixels.begin(), sorted_pixels.end());

  // write pixels back
  for (std::size_t i = 0; i < sorted_pixels.size(); i += 1)
  {
    int pixel_height = start + i;
    uint8_t *pixel =
        image.pixels + (pixel_height * image.width + column_index) *
                           image.channels; // 3 channels

    pixel[0] = sorted_pixels[i][0];
    pixel[1] = sorted_pixels[i][1];
    pixel[2] = sorted_pixels[i][2];
  }
}

int BaseImageSorter::get_hue(uint8_t r, uint8_t g, uint8_t b)
{
  float rf = r / 255.f, gf = g / 255.f, bf = b / 255.f;

  float min = std::min({rf, gf, bf});
  float max = std::max({rf, gf, bf});
  float diff = max - min;

  float hue;

  if (diff == 0)
  {
    return 0;
  }
  // r is max: (g-b)/diff
  else if (rf == max)
  {
    hue = (gf - bf) / diff;
  }
  // g is max: 2.0 + (b-r)/diff
  else if (gf == max)
  {
    hue = 2.f + (bf - rf) / diff;
  }
  // b is max: 4.0 + (r-g)/diff
  else
  {
    hue = 4.f + (rf - gf) / diff;
  }

  hue *= 60.f;
  if (hue < 0)
    hue += 360.f;

  return hue;
}
