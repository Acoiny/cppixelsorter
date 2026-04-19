#include "cli.hpp"
#include "Ui/logger.hpp"

#include <bitset>
#include <cstdint>
#include <print>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

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

uint8_t parseLoggerFlags(const std::string &arg)
{
  using std::ranges::views::split, std::ranges::views::transform,
      std::ranges::to;

  const std::unordered_map<std::string, uint8_t> bitmasks{
      {"info", 0b0001},  {"warn", 0b0010}, {"error", 0b0100},
      {"debug", 0b1000}, {"all", 0b1111},
  };

  uint8_t res = 0;

  // set all mode for now, so we can print errors during parsing
  UI::Logger::SetMode(UI::Logger::Mode::all);

  auto fields =
      arg | split(',') |
      // for every string
      transform(
          [](auto el)
          {
            // lowercase each character
            return el | transform([](char c) { return std::tolower(c); });
          }) |
      to<std::vector<std::string>>();

  for (const auto &f : fields)
  {
    if (bitmasks.contains(f))
    {
      const auto val = bitmasks.find(f);
      res |= val->second;
    }
    else
    {
      UI::Logger::Error("Ignored unknown logger flag '{}'", f);
    }
  }

  return res;
}
