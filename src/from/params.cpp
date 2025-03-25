#include <algorithm>
#include <array>
#include <chrono>
#include <spdlog/spdlog.h>
#include <thread>

#include "../modutils.hpp"
#include "params.hpp"

using namespace std;

from::params::ParamList **from::params::param_list_address = nullptr;

static auto required_params =
    array{L"EquipParamAccessory", L"EquipParamGem",        L"EquipParamGoods",
          L"EquipParamProtector", L"EquipParamWeapon",     L"ItemLotParam_enemy",
          L"ItemLotParam_map",    L"ReinforceParamWeapon", L"ShopLineupParam"};

void from::params::initialize()
{
    param_list_address = modutils::scan<ParamList *>({
        .aob = "48 8B 0D ?? ?? ?? ?? 48 85 C9 0F 84 ?? ?? ?? ?? 45 33 C0 BA 90",
        .relative_offsets = {{3, 7}},
    });

    spdlog::info("Waiting for params...");

    while (true)
    {
        auto param_list = *param_list_address;
        if (param_list != nullptr)
        {
            int required_param_count = 0;

            for (auto &entry : param_list->entries)
            {
                if (entry.param_res_cap != nullptr &&
                    ranges::find(required_params,
                                 wstring{dlw_c_str(&entry.param_res_cap->param_name)}) !=
                        required_params.end())
                {
                    required_param_count++;
                }

                if (required_param_count == required_params.size())
                {
                    return;
                }
            }
        }

        this_thread::sleep_for(chrono::seconds(1));
    }
}
