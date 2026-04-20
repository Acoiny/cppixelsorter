#include "Sorters/baseImageSorter.hpp"
#include "Ui/cursorManager.hpp"
#include "Ui/logger.hpp"
#include "cli.hpp"
#include "imageData.hpp"
#include "stb_image.h"

#include "gui.hpp"
#include "timer.hpp"

#include <cstdint>
#include <getopt.h>
#include <print>
#include <stdexcept>

int get_hue_from_rgb(unsigned char r, unsigned char g, unsigned char b);

void usage(const std::string &progname)
{
  std::println(stderr, R"(Usage: {} <infile> [OPTIONS]
    OPTIONAL:
        --hue       | -h <min>[:<max>]        Hue threshold.
        --outfile   | -o <file>         Output file name. Ending specifies filetype.
        --gui       | -g                Activates graphical user interface.
        --log-level <level>             Takes comma-separated log-levels:
                                            debug, info, warn, error, all
        --verbose   | -v                Sets log level to "all"
)",
               progname);
}

int gui_mode(const char *infile, const std::string &log_levels);

enum class APP_MODE
{
  NONE,
  CLI,
  GUI,
};

const struct option long_options[] = {
    {"hue", required_argument, nullptr, 'h'},
    {"outfile", required_argument, nullptr, 'o'},
    {"gui", no_argument, nullptr, 'g'},
    {"log-level", required_argument, nullptr, 'l'},
    {"verbose", no_argument, nullptr, 'v'},
    // last element
    {nullptr, 0, nullptr, 0},
};

void parse_flags(int argc, char *argv[]) {}

int main(int argc, char *argv[])
{
  int c;

  char *infile = nullptr;
  std::string outfile = "";
  std::string log_levels = "";

  int hue_min = 0;
  int hue_max = 360;

  APP_MODE mode = APP_MODE::CLI;

  while ((c = getopt_long(argc, argv, "gh:o:l:v", long_options, nullptr)) != -1)
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
      auto res = parseHue(optarg);
      hue_min = res.first;
      hue_max = res.second;
      break;
    }
    case 'o':
      outfile = optarg;
      break;
    case 'l':
      log_levels = optarg;
      break;
    case 'v':
      log_levels = "all";
      break;
    default:
      usage(argv[0]);
      return 1;
    }
  }

  // setting input file to positional argument (or null)
  infile = argv[optind];

  // GUI mode (automatically, if no arguments given)
  if (mode == APP_MODE::GUI || argc == 1)
    return gui_mode(infile, log_levels);
  // END GUI

  if (!infile)
  {
    usage(argv[0]);
    return 1;
  }

  ImageData image(infile);
  SortTask task{.image = image, .hue_values = {.min = hue_min, .max = hue_max}};
  BaseImageSorter sorter(task);

  Timer t = Timer();

  // img.sort_vertical_ttb(hue_value);
  image = sorter.RunTask();

  auto duration = t.get();

  std::println("Sorting took {0}", duration);

  try
  {
    if (outfile.size() == 0)
    {
      std::string f(infile);
      auto idx = f.find_last_of('.');
      if (idx == std::string::npos)
        throw std::runtime_error("Unable to determine infile type!");
      auto file_ending = f.substr(idx);

      outfile = "output" + file_ending;
    }

    if (!image.write_to_file(outfile))
      throw std::runtime_error("Unable to save file!");
  }
  catch (std::runtime_error &e)
  {
    std::println(stderr, "{}", e.what());
    return 1;
  }

  return 0;
}

int gui_mode(const char *infile, const std::string &log_levels)
{
  uint8_t log_flags = parseLoggerFlags(log_levels);

  UI::Logger::SetMode((UI::Logger::Mode)log_flags);

  // GUI
  Gui gui(1200, 600, "Pixelsorter");

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
