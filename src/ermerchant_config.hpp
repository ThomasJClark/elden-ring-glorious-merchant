#pragma once

#include <filesystem>

namespace ermerchant
{
/**
 * Load user preferences from an .ini file
 */
void load_config(const std::filesystem::path &ini_path);

namespace config
{
/**
 * Automatically sell weapons upgraded to the highest level you've gotten on a given character
 */
extern bool auto_upgrade_weapons;

};
};
