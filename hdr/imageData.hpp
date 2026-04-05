#pragma once

#include <SDL3/SDL_surface.h>
#include <cstdint>
#include <string>

/**
 * A struct containing an image's data
 * Must be freed!
 */
struct ImageData
{
  ImageData() = default;
  ImageData(const ImageData &other);
  ImageData &operator=(const ImageData &other);
  ImageData(const std::string &filepath, int requested_channels = 3);
  /**
   * The ImageData doesn't own the data, when created
   * using this constructor
   */
  // ImageData(SDL_Surface *surface, int n_channels);

  ~ImageData();

  void free();

  bool write_to_file(const std::string &filepath);

  SDL_Surface *toSurface();

  uint8_t *pixels = nullptr;
  int width = 0, height = 0;
  int channels = 0;

private:
  bool m_owns_data = true;
};
