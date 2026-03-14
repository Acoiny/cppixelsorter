#pragma once

#include <dbus/dbus.h>
#include <string>
#include <vector>

class Filepicker
{
  friend DBusHandlerResult
  response_filter(DBusConnection *conn, DBusMessage *message, void *user_data);

public:
  Filepicker() = default;

  /**
   * Opens the filepicker and returns true, if a file has been selected
   */
  bool open();

private:
  std::vector<std::string> m_filenames;
};
