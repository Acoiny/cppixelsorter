#include "Ui/Colors.hpp"

namespace UI
{
namespace Color
{
SDL_Color CLEAR_COLOR;

// button
SDL_Color BUTTON_BACKGROUND;
SDL_Color BUTTON_HOVER;
SDL_Color BUTTON_DOWN;

// font
SDL_Color FONT;

// slider
SDL_Color SLIDER_ACTIVE;
SDL_Color SLIDER_IDLE;

// checkbox
SDL_Color CHECKBOX_CHECKED;
SDL_Color CHECKBOX_UNCHECKED;

void LoadDevelopColorTheme()
{
  CLEAR_COLOR = {93, 59, 107, 255};

  BUTTON_BACKGROUND = {0x83, 0x3d, 0xb4, 255};
  BUTTON_HOVER = {0xa2, 0x55, 0xd9, 255};
  BUTTON_DOWN = {0xc3, 0x75, 0xf9, 255};

  FONT = {0, 0, 0, 255};

  SLIDER_ACTIVE = {0xc3, 0x75, 0xf9, 255};
  SLIDER_IDLE = {0x83, 0x3d, 0xb4, 255};

  CHECKBOX_CHECKED = {0, 255, 0, 255};
  CHECKBOX_UNCHECKED = {255, 0, 0, 255};
}

void LoadDefaultColorTheme()
{
  CLEAR_COLOR = {50, 50, 50, 255};

  BUTTON_BACKGROUND = {100, 100, 100, 255};
  BUTTON_HOVER = {80, 80, 80, 255};
  BUTTON_DOWN = {65, 65, 65, 255};

  FONT = {255, 255, 255, 255};

  SLIDER_ACTIVE = {65, 65, 65, 255};
  SLIDER_IDLE = {100, 100, 100, 255};

  CHECKBOX_CHECKED = {0, 255, 0, 255};
  CHECKBOX_UNCHECKED = {255, 0, 0, 255};
}

} // namespace Color
} // namespace UI
