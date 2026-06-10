#include "Ui/themes.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>

#include "Ui/Colors.hpp"
#include "Ui/config.hpp"
#include "Ui/logger.hpp"

static bool LoadFromFile(const std::string &filename);

bool UI::Theme::LoadFromConfig()
{
  const auto app_name = UI::Config::get_application_name();

  const auto paths = {std::format("./.{}.theme", app_name)};

  for (auto p : paths)
  {
    if (std::filesystem::exists(p))
    {
      if (LoadFromFile(p))
        return true;
    }
  }

  return false;
}

static void trim(std::string &line, const char *avoid = " \n\r")
{
  // ltrim
  line.erase(0, line.find_first_not_of(avoid));

  // rtrim
  line.erase(line.find_last_not_of(avoid) + 1);
}

static SDL_Color ParseColor(std::string value)
{
  if (!value.starts_with('#'))
  {
    UI::Logger::Warn("Illegal color value: '{}'", value);
    return {};
  }

  value.erase(0, 1);

  bool has_a = value.length() == 8;

  uint32_t col = std::stoi(value, nullptr, 16);

  SDL_Color res;

  if (has_a)
  {
    res.a = 0xff & col;
    col >>= 8;
  }
  else
  {
    res.a = 0xff;
  }

  res.b = 0xff & col;
  col >>= 8;
  res.g = 0xff & col;
  col >>= 8;
  res.r = 0xff & col;
  col >>= 8;

  return res;
}

static bool ApplyVariable(std::string variable, std::string value)
{
  std::transform(variable.begin(), variable.end(), variable.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  UI::Logger::Debug("'{}' = '{}'", variable, value);

#define match(str, var)                                                        \
  else if (variable == str)                                                    \
  {                                                                            \
    UI::Color::var = ParseColor(value);                                        \
  }

  if (variable == "clear")
  {
    UI::Color::CLEAR_COLOR = ParseColor(value);
  }
  // clang-format off
  match("outline", OUTLINE_COLOR)
  match("button_background", BUTTON_BACKGROUND)
  match("button_hover", BUTTON_HOVER)
  match("button_down", BUTTON_DOWN)
  match("font", FONT)
  match("text", FONT)
  match("slider_active", SLIDER_ACTIVE)
  match("slider_idle", SLIDER_IDLE)
  match("checkbox_checked", CHECKBOX_CHECKED)
  match("checkbox_unchecked", CHECKBOX_UNCHECKED)
#undef match
  return true;
  // clang-format on
}

static bool ParseLine(std::string line)
{
  trim(line);

  if (line.starts_with('#') || line.length() == 0)
    return true;

  auto eq = line.find('=');

  if (eq == std::string::npos)
    return false;

  auto var_name = line.substr(0, eq);
  auto val = line.substr(eq + 1);

  trim(var_name);
  trim(val);

  return ApplyVariable(var_name, val);
}

static bool LoadFromFile(const std::string &filename)
{
  std::ifstream file;
  file.open(filename);

  if (!file.good())
    return false;

  std::string line;
  while (std::getline(file, line))
  {
    if (!ParseLine(line))
      return false;
  }

  return true;
}
