#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <algorithm>
#include <getopt.h>
#include <string>
#include <vector>
#include <array>
#include <stdexcept>

int get_hue_from_rgb(unsigned char r, unsigned char g, unsigned char b);


int main(int argc, char *argv[])
{
  int c;

  char *infile = nullptr;
  int hue_value = 0;

  while ((c = getopt(argc, argv, "i:h:")) != -1)
  {
    switch (c)
    {
    case 'i':
      infile = optarg;
      break;
    case 'h':
    {
      int v = (std::atoi(optarg));
      if (v < 0 || v > 360)
      {
        fprintf(stderr, "Hue value must be between 0 and 360\n");
        return 1;
      }
      hue_value = v;
      break;
    }
    default:
      fprintf(stderr, "Usage: %s -i <input_file> -h <hue_value>\n", argv[0]);
      return 1;
    }
  }

  // Load an image file with 4 desired channels (RGBA)
  int width, height, channels_in_file;
  unsigned char *image =
      stbi_load(infile,            // filename
                &width,            // output for width
                &height,           // output for height
                &channels_in_file, // output for original channels in file
                3                  // desired number of channels (rgb)
      );

  if (image == nullptr)
  {
    printf("Failed to load image: %s\n", stbi_failure_reason());
    return 1;
  }

  // iterate over each column and sort it
  for (int i = 0; i < width; i++)
  {
    bool in_path = false;
    // while the pixels match the selected hue, we add them to the "path"
    std::vector<std::array<unsigned char, 3>> path;

    for (int j = 0; j < height; j++)
    {
      unsigned char *pixel = image + (j * width + i) * 3; // 3 channels (RGB)
      unsigned char r = pixel[0];
      unsigned char g = pixel[1];
      unsigned char b = pixel[2];

      int hue = get_hue_from_rgb(r, g, b);

      if (hue >= hue_value)
      {
        in_path = true;
        path.push_back({r, g, b});
      }
      else
      {
        if (in_path)
        {
          // sort the path by hue
          std::sort(path.begin(), path.end(), [](const std::array<unsigned char, 3> &a, const std::array<unsigned char, 3> &b)
                    { return get_hue_from_rgb(a[0], a[1], a[2]) < get_hue_from_rgb(b[0], b[1], b[2]); });

          // write the sorted path back to the image
          for (size_t k = 0; k < path.size(); k++)
          {
            unsigned char *sorted_pixel = image + ((j - path.size() + k) * width + i) * 3;
            sorted_pixel[0] = path[k][0];
            sorted_pixel[1] = path[k][1];
            sorted_pixel[2] = path[k][2];
          }
          in_path = false;
          path.clear();
        }
      }
    }
    if(in_path)
    {
      // sort the path by hue
      std::sort(path.begin(), path.end(), [](const std::array<unsigned char, 3> &a, const std::array<unsigned char, 3> &b)
                { return get_hue_from_rgb(a[0], a[1], a[2]) < get_hue_from_rgb(b[0], b[1], b[2]); });

      // write the sorted path back to the image
      for (size_t k = 0; k < path.size(); k++)
      {
        unsigned char *sorted_pixel = image + ((height - path.size() + k) * width + i) * 3;
        sorted_pixel[0] = path[k][0];
        sorted_pixel[1] = path[k][1];
        sorted_pixel[2] = path[k][2];
      }
    }
  }

  // write the image back to disk
  stbi_write_png("output.png", width, height, 3, image, width * 3);

  // Don't forget to free the memory when done
  stbi_image_free(image);

  return 0;
}

int get_hue_from_rgb(unsigned char r, unsigned char g, unsigned char b)
{
  float rf = r / 255.0f;
  float gf = g / 255.0f;
  float bf = b / 255.0f;

  float max_val = std::max({rf, gf, bf});
  float min_val = std::min({rf, gf, bf});
  float delta = max_val - min_val;

  if (delta < 1e-6f)
  {
    return 0;
  }

  float hue;
  if (max_val - rf < 1e-6f)
  {
    hue = std::fmod((gf - bf) / delta, 6.0f);
  }
  else if (max_val - gf < 1e-6f)
  {
    hue = 2.0f + (bf - rf) / delta;
  }
  else
  {
    hue = 4.0f + (rf - gf) / delta;
  }

  hue *= 60.0f;
  if (hue < 0)
  {
    hue += 360.0f;
  }

  return static_cast<int>(hue);
}