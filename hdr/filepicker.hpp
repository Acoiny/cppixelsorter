#pragma once

#include <functional>
#include <string>

class Filepicker
{
  friend void callback(void *userdata, const char *const *filelist, int filter);

public:
  Filepicker() = default;

  /**
   * Opens the filepicker and returns true, if a file has been selected
   */
  bool open(bool saving = false);

  std::function<void(const std::string &, bool)> onSelect;

private:
  bool m_isSaving = false;

  bool m_isOpen = false;
};
