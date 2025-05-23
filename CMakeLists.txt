cmake_minimum_required(VERSION 3.28.1)

set(CMAKE_GENERATOR_PLATFORM x64)

project(EldenRingMerchantMod
  VERSION   "1.1.13"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

include(FetchContent)

FetchContent_Declare(minhook
  GIT_REPOSITORY        https://github.com/TsudaKageyu/minhook.git
  GIT_TAG               f5485b8454544c2f034c78f8f127c1d03dea3636)

FetchContent_Declare(pattern16
  GIT_REPOSITORY        https://github.com/Dasaav-dsv/Pattern16.git
  GIT_TAG               728eac543cee7e7bf6fda445d7db06e3dc8a61d0
  CONFIGURE_COMMAND     ""
  BUILD_COMMAND         "")

FetchContent_Declare(mini
  GIT_REPOSITORY        https://github.com/metayeti/mINI.git
  GIT_TAG               0.9.15
  CONFIGURE_COMMAND     ""
  BUILD_COMMAND         "")

set(SPDLOG_DISABLE_DEFAULT_LOGGER ON)
set(SPDLOG_USE_STD_FORMAT ON)
FetchContent_Declare(spdlog
  GIT_REPOSITORY        https://github.com/gabime/spdlog.git
  GIT_TAG               v1.13.0)

set(SWSDK_VERSION "1.59")
set(SWSDK_RELEASE "x64")
FetchContent_Declare(steamworks-sdk
  URL                   https://github.com/julianxhokaxhiu/SteamworksSDKCI/releases/download/${SWSDK_VERSION}/SteamworksSDK-v${SWSDK_VERSION}.0_${SWSDK_RELEASE}.zip
  CONFIGURE_COMMAND     ""
  BUILD_COMMAND         "")

# # Set iterator debug level to 0 for ELDEN RING ABI compatibility
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)

FetchContent_MakeAvailable(minhook pattern16 mini spdlog steamworks-sdk)

add_library(pattern16 INTERFACE)
target_include_directories(pattern16 INTERFACE ${pattern16_SOURCE_DIR}/include)

add_library(mini INTERFACE)
target_include_directories(mini INTERFACE ${mini_SOURCE_DIR}/src)

add_library(steamapi STATIC IMPORTED GLOBAL)
set_property(TARGET steamapi APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(steamapi PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG CXX
        IMPORTED_LOCATION_DEBUG steam_api64.lib)

add_library(steamworks-sdk STATIC IMPORTED GLOBAL)
target_include_directories(steamworks-sdk INTERFACE ${steamworks-sdk_SOURCE_DIR}/include)
set_property(TARGET steamworks-sdk APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(steamworks-sdk PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG CXX
        IMPORTED_LOCATION_DEBUG ${steamworks-sdk_SOURCE_DIR}/lib/steam/steam_api64.lib)

add_library(EldenRingMerchantMod SHARED
  src/from/talk_commands.hpp
  src/from/param_lookup.hpp
  src/from/messages.hpp
  src/from/game_data.hpp
  src/from/ezstate.hpp
  src/from/params.hpp
  src/from/params.cpp
  src/modutils.hpp
  src/modutils.cpp
  src/ermerchant_config.hpp
  src/ermerchant_config.cpp
  src/ermerchant_talkscript.hpp
  src/ermerchant_talkscript.cpp
  src/ermerchant_talkscript_utils.hpp
  src/ermerchant_shops.hpp
  src/ermerchant_shops.cpp
  src/ermerchant_messages.hpp
  src/ermerchant_messages.cpp
  src/ermerchant_messages_by_lang.cpp
  src/dllmain.cpp)

set_target_properties(EldenRingMerchantMod PROPERTIES OUTPUT_NAME "ermerchant")

add_definitions(-DPROJECT_VERSION="${CMAKE_PROJECT_VERSION}")

add_custom_command(TARGET EldenRingMerchantMod POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:EldenRingMerchantMod>
  ${CMAKE_SOURCE_DIR}/LICENSE.txt
  ${CMAKE_SOURCE_DIR}/ermerchant.ini
  COMMAND_EXPAND_LISTS)

target_link_libraries(EldenRingMerchantMod
  minhook
  pattern16
  mini
  spdlog
  steamworks-sdk)
