#pragma once

#include <cstdio>
#include <exception>
#include <format>
#include <print>
#include <string_view>
#include <type_traits>
#include <utility>
namespace UI
{
class Logger
{
public:
  enum Mode : unsigned
  {
    none = 0,
    info = 0b1,
    warn = 0b10,
    error = 0b100,
    debug = 0b1000,

    // convenience flag
    all = 0b1111,
    // FATAL always gets logged (because program will terminate!)
  };

  friend constexpr Mode operator|(Mode lhs, Mode rhs)
  {
    return static_cast<Mode>(static_cast<unsigned>(lhs) |
                             static_cast<unsigned>(rhs));
  }

  /**
   * Sets the loggers mode to the given flags.
   * To enable multiple modes, use bitwise OR '|' with multiple flags.
   */
  static void SetMode(Mode mode) { s_mode = mode; }

  template <typename... Args>
  using FormatString =
      std::basic_format_string<char, std::type_identity_t<Args>...>;

  template <typename... Args>
  static void Info(FormatString<Args...> msg, Args &&...args)
  {
    if (HasMode(Mode::info))
      LogWithLabel("INFO", kBlue, msg, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void Warn(FormatString<Args...> msg, Args &&...args)
  {
    if (HasMode(Mode::warn))
      LogWithLabel("WARN", kYellow, msg, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void Error(FormatString<Args...> msg, Args &&...args)
  {
    if (HasMode(Mode::error))
      LogWithLabel("ERROR", kRed, msg, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void Debug(FormatString<Args...> msg, Args &&...args)
  {
    if (HasMode(Mode::debug))
      LogWithLabel("DEBUG", kCyan, msg, std::forward<Args>(args)...);
  }

  template <typename... Args>
  [[noreturn]] static void Fatal(FormatString<Args...> msg, Args &&...args)
  {
    LogWithLabel("FATAL", kMagenta, msg, std::forward<Args>(args)...);
    std::terminate();
  }

private:
  template <typename... Args>
  static void LogWithLabel(const char *label, std::string_view color,
                           FormatString<Args...> msg, Args &&...args)
  {
    std::println(stderr, "[{}{}{}]: {}", color, label, kReset,
                 std::format(msg, std::forward<Args>(args)...));
  }

  static bool HasMode(Mode flag)
  {
    return (static_cast<unsigned>(s_mode) & static_cast<unsigned>(flag)) != 0;
  }

  inline static constexpr std::string_view kBlue = "\x1b[34m";
  inline static constexpr std::string_view kYellow = "\x1b[33m";
  inline static constexpr std::string_view kRed = "\x1b[31m";
  inline static constexpr std::string_view kCyan = "\x1b[36m";
  inline static constexpr std::string_view kMagenta = "\x1b[35m";
  inline static constexpr std::string_view kReset = "\x1b[0m";

  inline static Mode s_mode = Mode::none;
};
} // namespace UI
