#include "imageSorter.hpp"
#include "stb_image.h"

#include "gui.hpp"

#include <chrono>
#include <getopt.h>
#include <print>
#include <stdexcept>

int get_hue_from_rgb(unsigned char r, unsigned char g, unsigned char b);

void usage(const std::string &progname)
{
  std::println(stderr,
               "Usage: {} -i <input_file> -h <hue_value> -o <output_file>",
               progname);
}

int main(int argc, char *argv[])
{
  int c;

  char *infile = nullptr;
  std::string outfile = "";

  int hue_value = 0;

  while ((c = getopt(argc, argv, "i:h:o:")) != -1)
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
        std::println(stderr, "Hue value must be between 0 and 360");
        return 1;
      }
      hue_value = v;
      break;
    }
    case 'o':
      outfile = optarg;
      break;
    default:
      usage(argv[0]);
      return 1;
    }
  }

  if (!infile)
  {
    usage(argv[0]);
    return 1;
  }

  // GUI
  Gui gui(620, 480, "Pixelsorter");
  gui.LoadImage(infile);

  while (!gui.ShouldClose())
  {
    gui.Update();
  }
  return 0;
  // END GUI

  ImageSorter img(infile);

  auto start = std::chrono::high_resolution_clock::now();

  img.sort_vertical(hue_value);

  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> duration = end - start;

  std::println("Sorting took {0}", duration);

  try
  {
    img.write_to_file(outfile);
  }
  catch (std::runtime_error &e)
  {
    std::println(stderr, "{}", e.what());
    return 1;
  }

  return 0;
}
