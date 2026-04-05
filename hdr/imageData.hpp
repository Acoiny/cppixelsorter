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
  ImageData(const std::string &filepath, int requested_channels = 3);
  ImageData(SDL_Surface *surface, int n_channels);

  void free();

  bool write_to_file(const std::string &filepath);

  uint8_t *pixels = nullptr;
  int width = 0, height = 0;
  int channels = 0;

private:
  bool m_owns_data = true;
};
