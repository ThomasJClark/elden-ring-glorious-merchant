#define WIN32_LEAN_AND_MEAN
#include <filesystem>
#include <memory>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <thread>
#include <windows.h>

#include "ermerchant_messages.hpp"
#include "ermerchant_shops.hpp"
#include "modutils.hpp"

static std::thread mod_thread;

static void setup_logger(const wchar_t *dll_filename)
{
    auto log_file = std::filesystem::path(dll_filename).parent_path() / "logs" / "ermerchant.log";

    auto logger = std::make_shared<spdlog::logger>("merchant");
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] %^[%l]%$ %v");
    logger->sinks().push_back(
        std::make_shared<spdlog::sinks::daily_file_sink_st>(log_file.string(), 0, 0, false, 5));
    logger->flush_on(spdlog::level::info);

// #if _DEBUG
#if 1
    AllocConsole();
    FILE *stream;
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);
    freopen_s(&stream, "CONIN$", "r", stdin);
    logger->sinks().push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
    logger->set_level(spdlog::level::trace);
#endif

    spdlog::set_default_logger(logger);
}

static void setup_mod()
{
    modutils::initialize();

    spdlog::info("Sleeping to work around a bug in Seamless Co-op...");
    std::this_thread::sleep_for(std::chrono::seconds(5));

    spdlog::info("Hooking messages...");
    ermerchant::setup_messages();

    spdlog::info("Hooking shops...");
    ermerchant::setup_shops();

    spdlog::info("Sleeping to work around a bug in Seamless Co-op...");
    std::this_thread::sleep_for(std::chrono::seconds(5));

    modutils::enable_hooks();
    spdlog::info("Initialized mod");
}

bool WINAPI DllMain(HINSTANCE dll_instance, unsigned int fdw_reason, void *lpv_reserved)
{
    if (fdw_reason == DLL_PROCESS_ATTACH)
    {
        wchar_t dll_filename[MAX_PATH] = {0};
        GetModuleFileNameW(dll_instance, dll_filename, MAX_PATH);
        setup_logger(dll_filename);

#ifdef PROJECT_VERSION
        spdlog::info("Glorious Merchant version {}", PROJECT_VERSION);
#endif

        mod_thread = std::thread([]() {
            try
            {
                setup_mod();
            }
            catch (std::runtime_error const &e)
            {
                spdlog::error("Error initializing mod: {}", e.what());
                modutils::deinitialize();
            }
        });
    }
    else if (fdw_reason == DLL_PROCESS_DETACH && lpv_reserved != nullptr)
    {
        try
        {
            mod_thread.join();
            modutils::deinitialize();
            spdlog::info("Deinitialized mod");
        }
        catch (std::runtime_error const &e)
        {
            spdlog::error("Error deinitializing mod: {}", e.what());
            spdlog::shutdown();
            return false;
        }
        spdlog::shutdown();
    }
    return true;
}