#include "filepicker.hpp"
#include <sui/logger.hpp>

#include <SDL3/SDL.h>

static const SDL_DialogFileFilter filters[] = {{"All images", "png;jpg;jpeg"},
                                               {"PNG images", "png"},
                                               {"JPEG images", "jpg;jpeg"},
                                               {"All files", "*"}};

void SDLCALL callback(void *userdata, const char *const *filelist,
                      [[maybe_unused]] int filter)
{
  Filepicker *fp = (Filepicker *)userdata;
  fp->m_isOpen = false;

  if (!filelist)
  {
    sui::Logger::Error("An error occured: {}", SDL_GetError());
    return;
  }
  else if (!*filelist)
  {
    sui::Logger::Debug("File dialog cancelled");
    return;
  }

  while (*filelist)
  {
    sui::Logger::Info("Full path to selected file: '{}'", *filelist);

    if (fp->onSelect)
      fp->onSelect(*filelist, fp->m_isSaving);

    filelist++;
  }
}

bool Filepicker::open(bool saving)
{
  if (m_isOpen)
    return false;

  m_isSaving = saving;
  if (saving)
    SDL_ShowSaveFileDialog(callback, this, nullptr, filters, 4, nullptr);
  else
    SDL_ShowOpenFileDialog(callback, this, nullptr, filters, 4, nullptr, false);

  m_isOpen = true;

  return true;
}
