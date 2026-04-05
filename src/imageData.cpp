#include "imageData.hpp"

#include "Ui/logger.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>

ImageData::ImageData(const std::string &filepath, int requested_channels)
{
  pixels = (stbi_load(filepath.c_str(), &width, &height, &channels,
                      requested_channels));
  channels = requested_channels;
}

ImageData::ImageData(const ImageData &other)
{
  std::size_t len =
      other.height * other.width * other.channels * sizeof(uint8_t);
  pixels = (uint8_t *)::malloc(len);

  if (pixels == nullptr)
    throw std::runtime_error("Unable to allocate memory for image!");

  ::memcpy(pixels, other.pixels, len);

  width = other.width;
  height = other.height;
  channels = other.channels;
  m_owns_data = true;
}

ImageData &ImageData::operator=(const ImageData &other)
{
  std::size_t curr_len = width * height * channels;
  std::size_t other_len = other.width * other.height * other.channels;
  // we need more memory
  if (curr_len < other_len)
  {
    void *newPtr = ::realloc(pixels, other_len);

    if (newPtr == nullptr)
      throw std::runtime_error("Unable to allocate memory for image!");

    pixels = (uint8_t *)newPtr;
  }

  ::memcpy(pixels, other.pixels, other_len);

  width = other.width;
  height = other.height;
  channels = other.channels;
  m_owns_data = true;

  return *this;
}

// ImageData::ImageData(SDL_Surface *surface, int n_channels)
// {
//   pixels = (uint8_t *)surface->pixels;
//   width = surface->w;
//   height = surface->h;
//   channels = n_channels;
//
//   m_owns_data = false;
// }

ImageData::~ImageData() { free(); }

void ImageData::free()
{
  // only free, if the image data object owns the image
  if (m_owns_data && pixels)
    ::free(pixels);
}

bool ImageData::write_to_file(const std::string &filepath)
{
  std::println("writing to {}", filepath);
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

SDL_Surface *ImageData::toSurface()
{
  // TODO: check if "pitch" is correct
  SDL_Surface *surface = SDL_CreateSurfaceFrom(
      width, height, SDL_PIXELFORMAT_RGB24, pixels, width * 3);

  if (!surface)
  {
    throw std::runtime_error(
        std::format("Unable to create surface: {}", SDL_GetError()));
  }

  return surface;
}
