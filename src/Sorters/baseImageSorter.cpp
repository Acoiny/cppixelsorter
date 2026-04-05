#include "Sorters/baseImageSorter.hpp"

#include <algorithm>

BaseImageSorter::BaseImageSorter(ImageData image) : m_image(image) {}

int BaseImageSorter::get_hue(uint8_t r, uint8_t g, uint8_t b)
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
