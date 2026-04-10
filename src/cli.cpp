#include "cli.hpp"

#include <print>
#include <string>

std::pair<int, int> parseHue(const std::string &arg)
{
  auto res = std::pair(0, 360);

  // two hue values detected
  if (auto idx = arg.find(':'); idx != std::string_view::npos)
  {
    auto first = arg.substr(0, idx);
    auto second = arg.substr(idx + 1);
    res = std::pair(std::stoi(first), std::stoi(second));
  }
  else
  {
    res.first = std::stoi(arg);
  }

  auto inrange = [](int val) { return val >= 0 && val <= 360; };

  if (!inrange(res.first) || !inrange(res.second))
  {
    std::println(stderr, "Illegal value for hue!\nValue must in range 0-360");
    std::exit(1);
  }

  return res;
}
