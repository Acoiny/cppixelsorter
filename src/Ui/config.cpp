#include "Ui/config.hpp"

#include <string>

static std::string s_application_name;

void UI::Config::set_application_name(const std::string &name)
{
  s_application_name = name;
}

const std::string_view UI::Config::get_application_name()
{
  return s_application_name;
}
