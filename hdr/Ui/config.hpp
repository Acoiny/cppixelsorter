#pragma once

#include <string>
#include <string_view>

namespace UI
{
namespace Config
{
/**
 * Sets the application name. This is used for things
 * like finding the config files
 */
void set_application_name(const std::string &name);

const std::string_view get_application_name();
} // namespace Config
} // namespace UI
