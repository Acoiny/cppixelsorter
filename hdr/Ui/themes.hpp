#pragma once

namespace UI
{
namespace Theme
{
/**
 * Loads the theme from a config file, searches for the following
 * config files in order:
 * - ./.cppixelsorter.theme
 * - ~/.local/config/cppixelsorter/theme.conf
 *
 * @returns true, if a config file was found and correctly parsed
 */
bool LoadFromConfig();
} // namespace Theme
} // namespace UI
