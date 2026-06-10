#pragma once

#include <SDL3/SDL_pixels.h>
namespace UI
{
namespace Color
{
extern SDL_Color CLEAR_COLOR;

// outline color (e.g. button outline on hover)
extern SDL_Color OUTLINE_COLOR;

// button
extern SDL_Color BUTTON_BACKGROUND;
extern SDL_Color BUTTON_HOVER;
extern SDL_Color BUTTON_DOWN;

// font
extern SDL_Color FONT;

// slider
extern SDL_Color SLIDER_ACTIVE;
extern SDL_Color SLIDER_IDLE;

// checkbox
extern SDL_Color CHECKBOX_CHECKED;
extern SDL_Color CHECKBOX_UNCHECKED;

void LoadDevelopColorTheme();
void LoadDefaultColorTheme();
}; // namespace Color
} // namespace UI
