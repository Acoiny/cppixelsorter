#include "Ui/themes.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <print>
#include <ranges>
#include <string>

#include "Ui/Colors.hpp"

static bool LoadFromFile(const std::string &filename);

bool UI::Theme::LoadFromConfig()
{
  const auto paths = {"./.cppixelsorter.theme"};

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

static bool ApplyVariable(std::string variable, std::string value)
{
  std::transform(variable.begin(), variable.end(), variable.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (variable == "clear")
  {
    // TODO: continue here
  }
  return true;
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
