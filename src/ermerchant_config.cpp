#include "ermerchant_config.hpp"

#include <mini/ini.h>
#include <spdlog/spdlog.h>

extern bool ermerchant::config::auto_upgrade_weapons = true;

void ermerchant::load_config(const std::filesystem::path &ini_path)
{
    spdlog::info("Loading config from {}", ini_path.string());

    mINI::INIFile file(ini_path.string());
    mINI::INIStructure ini;
    if (file.read(ini) && ini.has("ermerchant"))
    {
        auto &config = ini["ermerchant"];

        if (config.has("auto_upgrade_weapons"))
            config::auto_upgrade_weapons = config["auto_upgrade_weapons"] != "false";

        spdlog::info("auto_upgrade_weapons = {}", config::auto_upgrade_weapons);
    }
}
