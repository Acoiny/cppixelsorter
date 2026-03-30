#include "imageSorter.hpp"
#include <cstdint>
#include <numeric>
#include <print>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <format>
#include <stdexcept>

#include <algorithm>
#include <execution>
#include <ranges>

constexpr auto CHANNELS = 3;

int get_hue(uint8_t r, uint8_t g, uint8_t b);

ImageSorter::ImageSorter(const std::string &filename)
    : m_image(stbi_load(filename.c_str(), &m_width, &m_height, &m_channels,
                        CHANNELS)),
      m_filename(filename) // for now only RGB
{
  if (!m_image)
  {
    throw std::runtime_error(
        std::format("Unable to load image: {}", stbi_failure_reason()));
  }
}

ImageSorter::ImageSorter(SDL_Surface *surface, int n_channels)
    : m_image((uint8_t *)surface->pixels), m_width(surface->w),
      m_height(surface->h), m_channels(n_channels), m_destroy_image(false)
{
}

ImageSorter::~ImageSorter()
{
  if (m_destroy_image && m_image)
    stbi_image_free(m_image);
}

void ImageSorter::sort_column(int column_index, int start, int end,
                              std::array<int, 360> &hues)
{
  std::vector<std::array<uint8_t, 3>> sorted_pixels;

  for (int i = 1; i < hues.size(); i++)
  {
    hues[i] += hues[i - 1];
  }

  sorted_pixels.resize((end - start), {0});

  for (int i = start; i < end; i++)
  {
    uint8_t *pixel =
        m_image + (i * m_width + column_index) * CHANNELS; // 3 channels
    uint8_t r = pixel[0], g = pixel[1], b = pixel[2];

    int hue = get_hue(r, g, b);

    hues[hue]--;
    sorted_pixels[hues[hue]] = {r, g, b};
  }

  // write pixels back
  for (int i = 0; i < sorted_pixels.size(); i += 1)
  {
    int pixel_height = start + i;
    uint8_t *pixel = m_image + (pixel_height * m_width + column_index) *
                                   CHANNELS; // 3 channels

    pixel[0] = sorted_pixels[i][0];
    pixel[1] = sorted_pixels[i][1];
    pixel[2] = sorted_pixels[i][2];
  }
}

void ImageSorter::sort_vertical(int hue_value)
{
  auto indices = std::views::iota(0);

  // get begin iterator and a matching end iterator by advancing begin
  auto b = indices.begin();
  auto e = std::next(b, m_width); // e has same iterator type as b

  std::for_each(std::execution::par, b, e,
                [&](int w)

                // for (int w = 0; w < m_width; w++)
                {
                  std::array<int, 360> counts = {0};

                  int path_start = -1;

                  for (int h = 0; h < m_height; h++)
                  {
                    uint8_t *pixel =
                        m_image + (h * m_width + w) * CHANNELS; // 3 channels
                    uint8_t r = pixel[0], g = pixel[1], b = pixel[2];

                    int hue = get_hue(r, g, b);

                    if (hue >= hue_value)
                    {
                      // if NOT in path
                      if (path_start == -1)
                      {
                        // start path
                        path_start = h;
                      }
                      counts[hue]++;
                    }
                    // pixel is NOT valid anymore
                    else
                    {
                      // we have a path!
                      if (path_start != -1)
                      {
                        int path_end = h;

                        sort_column(w, path_start, path_end, counts);
                        counts.fill(0);

                        path_start = -1;
                      }
                    }
                  }
                  if (path_start != -1)
                  {
                    // we have a path!
                    int path_end = m_height;

                    sort_column(w, path_start, path_end, counts);

                    path_start = -1;
                  }
                });
}

void ImageSorter::write_to_file(const std::string &filename)
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

  if (outfile_name.ends_with(".png"))
  {
    std::println("Writing image to {}", outfile_name);
    stbi_write_png(outfile_name.c_str(), m_width, m_height, CHANNELS, m_image,
                   m_width * 3);
  }
  else if (outfile_name.ends_with(".jpg") || outfile_name.ends_with(".jpeg"))
  {
    std::println("Writing image to {}", outfile_name);
    stbi_write_jpg(outfile_name.c_str(), m_width, m_height, CHANNELS, m_image,
                   100);
  }
  else
  {
    throw std::runtime_error(
        std::format("Unknown file ending: {}", outfile_name));
  }
}

int get_hue(uint8_t r, uint8_t g, uint8_t b)
{
  float rf = r / 255.f, gf = g / 255.f, bf = b / 255.f;

  float min = std::min({rf, gf, bf});
  float max = std::max({rf, gf, bf});
  float diff = max - min;

  float hue;

  // r is max: (g-b)/diff
  if (rf == max)
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
