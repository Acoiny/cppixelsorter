#include "imageSorter.hpp"
#include "stb_image.h"

#include "gui.hpp"
#include "timer.hpp"

#include <chrono>
#include <getopt.h>
#include <print>
#include <stdexcept>

int get_hue_from_rgb(unsigned char r, unsigned char g, unsigned char b);

void usage(const std::string &progname)
{
  std::println(stderr, R"(Usage: {} -i <input_file> [OPTIONS]
    OPTIONAL:
        -h <value>      Hue threshold.
        -o <file>       Output file name. Ending specifies filetype.
        -g              Activates graphical user interface.
)",
               progname);
}

int gui_mode(const char *infile);

enum class APP_MODE
{
  NONE,
  CLI,
  GUI,
};

int main(int argc, char *argv[])
{
  int c;

  char *infile = nullptr;
  std::string outfile = "";

  int hue_value = 0;

  APP_MODE mode = APP_MODE::CLI;

  while ((c = getopt(argc, argv, "gi:h:o:")) != -1)
  {
    switch (c)
    {
    case 'g':
    {
      mode = APP_MODE::GUI;
      break;
    }
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

  // GUI
  if (mode == APP_MODE::GUI)
    return gui_mode(infile);
  // END GUI

  if (!infile)
  {
    usage(argv[0]);
    return 1;
  }

  ImageSorter img(infile);

  Timer t = Timer();

  img.sort_vertical(hue_value);

  auto duration = t.get();

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

int gui_mode(const char *infile)
{
  // GUI
  Gui gui(620, 480, "Pixelsorter");

  if (infile)
  {
    gui.LoadImage(infile);
  }

  while (!gui.ShouldClose())
  {
    gui.Update();
  }
  return 0;
}
