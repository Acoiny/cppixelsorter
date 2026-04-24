#include "Ui/logger.hpp"
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

#include "gui.hpp"

Gui gui(1200, 600, "Pixelsorter");

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
  UI::Logger::SetMode(UI::Logger::Mode::all);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
  gui.Update();

  // if (gui.ShouldClose())
  //   return SDL_APP_SUCCESS;

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
  if (event->type == SDL_EVENT_QUIT)
    return SDL_APP_SUCCESS;

  if (gui.m_uiManager->isEventRelevant(*event))
  {
    gui.m_uiManager->handleEvent(*event);
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
