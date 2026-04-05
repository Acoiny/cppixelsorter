#include <cstdint>

/**
 * Specifies, what should be used to compare the pixels
 */
enum class PIXEL_CRITERIA
{
  HUE,
  SATURATION,
  BRIGHTNESS,
};

/**
 * A sorting task, that can be handled by the imagesorter class
 */
struct SortTask
{
  uint8_t *pixeldata;
  int width;
  int height;
  int channels;

  // criterion, by which spans get created
  PIXEL_CRITERIA selection_criterion = PIXEL_CRITERIA::HUE;
  // criterion, by which spans get sorted
  PIXEL_CRITERIA sort_criterion = PIXEL_CRITERIA::BRIGHTNESS;
};
