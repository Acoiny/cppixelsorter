#include "imageData.hpp"

#include "Ui/logger.hpp"
#include "stb_image.h"
#include "stb_image_write.h"
#include <cstdint>

ImageData::ImageData(const std::string &filepath, int requested_channels)
{
  pixels = (stbi_load(filepath.c_str(), &width, &height, &channels,
                      requested_channels));
  channels = requested_channels;
}
ImageData::ImageData(SDL_Surface *surface, int n_channels)
{
  pixels = (uint8_t *)surface->pixels;
  width = surface->w;
  height = surface->h;
  channels = n_channels;

  m_owns_data = false;
}

void ImageData::free()
{
  // only free, if the image data object owns the image
  if (m_owns_data && pixels)
    stbi_image_free(pixels);
}

bool ImageData::write_to_file(const std::string &filepath)
{
  if (filepath.ends_with(".jpg") || filepath.ends_with(".jpeg"))
  {
    UI::Logger::Info("Writing to file: {}", filepath);
    return 0 != stbi_write_jpg(filepath.c_str(), width, height, channels,
                               pixels, 100);
  }
  else if (filepath.ends_with(".png"))
  {
    UI::Logger::Info("Writing to file: {}", filepath);
    return 0 != stbi_write_png(filepath.c_str(), width, height, channels,
                               pixels, width * channels);
  }

  UI::Logger::Error("Unable to safe to file: {}", filepath);
  return false;
}
