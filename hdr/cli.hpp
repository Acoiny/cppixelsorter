#pragma once

#include <cstdint>
#include <string>
#include <utility>

std::pair<int, int> parseHue(const std::string &arg);

uint8_t parseLoggerFlags(const std::string &arg);
