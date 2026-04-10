#pragma once

#include "imageData.hpp"

/**
 * Specifies, what should be used to compare the pixels
 */
enum class PIXEL_CRITERIA
{
  HUE,
  SATURATION,
  BRIGHTNESS,
};

enum class SORT_DIRECTION
{
  VERTICAL_TTB,
  VERTICAL_BTT,
  HORIZON_LTR,
  HORIZON_RTL,
  // TODO: add more sorting behaviours
};

/**
 * A sorting task, that can be handled by the imagesorter class
 */
struct SortTask
{
  // reference, to the image that will be sorted
  ImageData &image;

  struct
  {
    int min = 0;
    int max = 360;
  } hue_values;

  // criterion, by which spans get created
  PIXEL_CRITERIA selection_criterion = PIXEL_CRITERIA::HUE;
  // criterion, by which spans get sorted
  PIXEL_CRITERIA sort_criterion = PIXEL_CRITERIA::BRIGHTNESS;
  // basically the "path" the spans lie on
  SORT_DIRECTION sort_direction = SORT_DIRECTION::VERTICAL_TTB;
};
