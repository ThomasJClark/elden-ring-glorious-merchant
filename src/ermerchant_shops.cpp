/**
 * ermerchant_shop.cpp
 *
 * New shop params. This iterates through every obtainable item in the game, and creates a shop
 * param in the appropriate shop that allows buying it for free. The shop lookup functions are
 * hooked in order to return these modded params.
 */
#include "ermerchant_shops.hpp"

#include <array>
#include <set>
#include <vector>

#include "from/paramdef/EQUIP_PARAM_ACCESSORY_ST.hpp"
#include "from/paramdef/EQUIP_PARAM_GEM_ST.hpp"
#include "from/paramdef/EQUIP_PARAM_GOODS_ST.hpp"
#include "from/paramdef/EQUIP_PARAM_PROTECTOR_ST.hpp"
#include "from/paramdef/EQUIP_PARAM_WEAPON_ST.hpp"
#include "from/paramdef/ITEMLOT_PARAM_ST.hpp"
#include "from/paramdef/SHOP_LINEUP_PARAM.hpp"

#include "ermerchant_messages.hpp"
#include "from/game_data.hpp"
#include "from/param_lookup.hpp"
#include "from/params.hpp"
#include "modutils.hpp"

static const std::wstring cut_content_prefix = L"[ERROR]";

static constexpr unsigned char lot_item_category_goods = 1;

static constexpr unsigned char equip_type_weapon = 0;
static constexpr unsigned char equip_type_protector = 1;
static constexpr unsigned char equip_type_accessory = 2;
static constexpr unsigned char equip_type_goods = 3;
static constexpr unsigned char equip_type_gem = 4;

static constexpr unsigned char weapon_type_arrow = 81;
static constexpr unsigned char weapon_type_greatarrow = 83;
static constexpr unsigned char weapon_type_bolt = 85;
static constexpr unsigned char weapon_type_ballista_bolt = 86;

static constexpr long long weapon_unarmed_id = 110000;

static constexpr unsigned char protector_category_head = 0;
static constexpr unsigned char protector_category_chest = 1;
static constexpr unsigned char protector_category_arms = 2;
static constexpr unsigned char protector_category_legs = 3;

static constexpr long long protector_bare_head_id = 10000;
static constexpr long long protector_bare_chest_id = 10100;
static constexpr long long protector_bare_arms_id = 10200;
static constexpr long long protector_bare_legs_id = 10300;

static constexpr long long goods_memory_of_grace_id = 115;
static constexpr long long goods_phantom_great_rune_id = 135;
static constexpr long long goods_flask_begin_id = 1000;
static constexpr long long goods_flask_end_id = 1076;
static constexpr long long goods_golden_seed_id = 10010;
static constexpr long long goods_sacred_tear_id = 10020;

static constexpr unsigned char goods_type_normal_item = 0;
static constexpr unsigned char goods_type_key_item = 1;
static constexpr unsigned char goods_type_crafting_material = 2;
static constexpr unsigned char goods_type_remembrance = 3;
static constexpr unsigned char goods_type_sorcery = 5;
static constexpr unsigned char goods_type_spirit_summon_lesser = 7;
static constexpr unsigned char goods_type_spirit_summon_greater = 8;
static constexpr unsigned char goods_type_wondrous_physick = 9;
static constexpr unsigned char goods_type_wondrous_physick_tear = 10;
static constexpr unsigned char goods_type_regenerative_material = 11;
static constexpr unsigned char goods_type_info_item = 12;
static constexpr unsigned char goods_type_reinforcement_material = 14;
static constexpr unsigned char goods_type_great_rune = 15;
static constexpr unsigned char goods_type_incantation = 16;
static constexpr unsigned char goods_type_self_buff_sorcery = 17;
static constexpr unsigned char goods_type_self_buff_incantation = 18;

static constexpr unsigned char goods_sort_group_tutorial = 20;
static constexpr unsigned char goods_sort_group_gesture = 250;

static from::CS::GameDataMan **game_data_man_addr;

struct shop
{
    long long id;
    std::vector<from::paramdef::SHOP_LINEUP_PARAM> lineups;
};

static std::array<shop, 22> mod_shops = {
    shop{.id = ermerchant::shops::weapons},
    shop{.id = ermerchant::shops::armor},
    shop{.id = ermerchant::shops::spells},
    shop{.id = ermerchant::shops::talismans},
    shop{.id = ermerchant::shops::ammunition},
    shop{.id = ermerchant::shops::ashes_of_war},
    shop{.id = ermerchant::shops::spirit_summons},
    shop{.id = ermerchant::shops::consumables},
    shop{.id = ermerchant::shops::materials},
    shop{.id = ermerchant::shops::miscellaneous_items},
    shop{.id = ermerchant::shops::cut_goods},
    shop{.id = ermerchant::shops::cut_armor},
    shop{.id = ermerchant::shops::dlc_weapons},
    shop{.id = ermerchant::shops::dlc_armor},
    shop{.id = ermerchant::shops::dlc_spells},
    shop{.id = ermerchant::shops::dlc_talismans},
    shop{.id = ermerchant::shops::dlc_ammunition},
    shop{.id = ermerchant::shops::dlc_ashes_of_war},
    shop{.id = ermerchant::shops::dlc_spirit_summons},
    shop{.id = ermerchant::shops::dlc_consumables},
    shop{.id = ermerchant::shops::dlc_materials},
    shop{.id = ermerchant::shops::dlc_miscellaneous_items},
};

static from::find_shop_menu_result *(*solo_param_repository_lookup_shop_menu)(
    from::find_shop_menu_result *result, unsigned char shop_type, int begin_id, int end_id);

/**
 * Hook for SoloParamRepositoryImp::LookupShopMenu()
 *
 * Return a shop menu for the Glorious Merchant shop, or fall back to the vanilla shop menus.
 */
static from::find_shop_menu_result *solo_param_repository_lookup_shop_menu_detour(
    from::find_shop_menu_result *result, unsigned char shop_type, int begin_id, int end_id)
{
    for (auto &shop : mod_shops)
    {
        if (begin_id == shop.id)
        {
            result->shop_type = shop_type;
            result->id = begin_id;
            result->row = &shop.lineups[0];
            return result;
        }
    }

    return solo_param_repository_lookup_shop_menu(result, shop_type, begin_id, end_id);
}

static void (*solo_param_repository_lookup_shop_lineup)(from::find_shop_menu_result *,
                                                        unsigned char, int id);

/**
 * Hook for SoloParamRepositoryImp::LookupShopLineup()
 *
 * Return a shop lineup added by the mod to buy an item for $0, or fall back to the vanilla shop
 * lineups.
 */
static void solo_param_repository_lookup_shop_lineup_detour(from::find_shop_menu_result *result,
                                                            unsigned char shop_type, int id)
{
    for (auto &shop : mod_shops)
    {
        if (id >= shop.id && id < shop.id + ermerchant::shop_capacity)
        {
            if (id >= shop.id + shop.lineups.size())
            {
                break;
            }

            result->shop_type = shop_type;
            result->id = id;
            result->row = &shop.lineups[id - shop.id];
            return;
        }
    }

    solo_param_repository_lookup_shop_lineup(result, shop_type, id);
}

static void (*open_regular_shop)(void *, long long, long long);

/**
 * Hook for OpenRegularShop()
 *
 * Change the default sort order when opening one of the shops added by this mod.
 */
static void open_regular_shop_detour(void *unk, long long begin_id, long long end_id)
{
    open_regular_shop(unk, begin_id, end_id);

    for (auto &shop : mod_shops)
    {
        if (begin_id == shop.id)
        {
            auto game_data_man = *game_data_man_addr;
            if (game_data_man != nullptr)
            {
                for (auto &sort : game_data_man->menu_system_save_load->sorts)
                {
                    sort = from::menu_sort::item_type_ascending;
                }
            }
        }
    }
}

void ermerchant::setup_shops()
{
    auto &weapon_lineups = mod_shops[0].lineups;
    auto &armor_lineups = mod_shops[1].lineups;
    auto &spell_lineups = mod_shops[2].lineups;
    auto &talisman_lineups = mod_shops[3].lineups;
    auto &ammunition_lineups = mod_shops[4].lineups;
    auto &ash_of_war_lineups = mod_shops[5].lineups;
    auto &spirit_summon_lineups = mod_shops[6].lineups;
    auto &consumable_lineups = mod_shops[7].lineups;
    auto &material_lineups = mod_shops[8].lineups;
    auto &miscellaneous_item_lineups = mod_shops[9].lineups;
    auto &cut_good_lineups = mod_shops[10].lineups;
    auto &cut_armor_lineups = mod_shops[11].lineups;
    auto &dlc_weapon_lineups = mod_shops[12].lineups;
    auto &dlc_armor_lineups = mod_shops[13].lineups;
    auto &dlc_spell_lineups = mod_shops[14].lineups;
    auto &dlc_talisman_lineups = mod_shops[15].lineups;
    auto &dlc_ammunition_lineups = mod_shops[16].lineups;
    auto &dlc_ashes_of_war_lineups = mod_shops[17].lineups;
    auto &dlc_spirit_summon_lineups = mod_shops[18].lineups;
    auto &dlc_consumable_lineups = mod_shops[19].lineups;
    auto &dlc_material_lineups = mod_shops[20].lineups;
    auto &dlc_miscellaneous_item_lineups = mod_shops[21].lineups;

    // Look up event flags set when acquiring items like maps and cookbooks. Simply possessing
    // these items doesn't actually unlock anything, an event flag must also be set.
    std::map<int, unsigned int> auto_goods_flags;
    for (auto param : {L"ItemLotParam_map", L"ItemLotParam_enemy"})
    {
        for (auto [id, row] : from::params::get_param<from::paramdef::ITEMLOT_PARAM_ST>(param))
        {
            if (row.lotItemCategory01 == lot_item_category_goods && row.getItemFlagId > 0)
            {
                auto_goods_flags[row.lotItemId01] = row.getItemFlagId;
            }
        }
    }
    for (auto [id, row] :
         from::params::get_param<from::paramdef::SHOP_LINEUP_PARAM>(L"ShopLineupParam"))
    {
        if (row.equipType == equip_type_goods)
        {
            auto_goods_flags[row.equipId] = row.eventFlag_forStock;
        }
    }

    // Iterate through every obtainable item in the game and create shop lineups in the appropriate
    // ranges
    for (auto [id, row] :
         from::params::get_param<from::paramdef::EQUIP_PARAM_WEAPON_ST>(L"EquipParamWeapon"))
    {
        // Exclude unarmed fist
        if (id == weapon_unarmed_id)
        {
            continue;
        }

        // Exclude duplicate weapon entries for heavy, keen, etc.
        auto affinity_id = (id % 10000) / 100;
        if (affinity_id != 0)
        {
            continue;
        }

        bool is_dlc = false;
        auto weapon_name = ermerchant::get_message(from::msgbnd::weapon_name, id);
        if (weapon_name.empty())
        {
            is_dlc = true;
            weapon_name = ermerchant::get_message(from::msgbnd::dlc_weapon_name, id);
        }

        // Exclude weapon entries without valid names - these are placeholders for data used by
        // non-weapons (e.g. perfumes) or unused/cut items.
        if (weapon_name.empty() || weapon_name.starts_with(cut_content_prefix))
        {
            continue;
        }

        row.sellValue = 0;

        std::vector<from::paramdef::SHOP_LINEUP_PARAM> *lineups = nullptr;

        if (row.wepType == weapon_type_arrow || row.wepType == weapon_type_greatarrow ||
            row.wepType == weapon_type_bolt || row.wepType == weapon_type_ballista_bolt)
        {

            if (is_dlc)
            {
                lineups = &dlc_ammunition_lineups;
            }
            else
            {
                lineups = &ammunition_lineups;
            }
        }
        else if (is_dlc)
        {
            lineups = &dlc_weapon_lineups;
        }
        else
        {
            lineups = &weapon_lineups;
        }

        lineups->push_back({.equipId = (int)id, .equipType = equip_type_weapon});
    }

    for (auto [id, row] :
         from::params::get_param<from::paramdef::EQUIP_PARAM_PROTECTOR_ST>(L"EquipParamProtector"))
    {
        // Exclude bare unarmored head/chest/etc.
        if (id == protector_bare_head_id || id == protector_bare_chest_id ||
            id == protector_bare_arms_id || id == protector_bare_legs_id)
        {
            continue;
        }

        // Exclude protector entries other than armor (e.g. hair)
        if (row.protectorCategory != protector_category_head &&
            row.protectorCategory != protector_category_chest &&
            row.protectorCategory != protector_category_arms &&
            row.protectorCategory != protector_category_legs)
        {
            continue;
        }

        bool is_dlc = false;
        auto protector_name = ermerchant::get_message(from::msgbnd::protector_name, id);
        if (protector_name.empty())
        {
            is_dlc = true;
            protector_name = ermerchant::get_message(from::msgbnd::dlc_protector_name, id);
        }

        if (protector_name.empty() || protector_name == cut_content_prefix)
        {
            continue;
        }

        row.sellValue = 0;

        std::vector<from::paramdef::SHOP_LINEUP_PARAM> *lineups = nullptr;

        if (protector_name.starts_with(cut_content_prefix))
        {
            lineups = &cut_armor_lineups;
        }
        else if (is_dlc)
        {
            lineups = &dlc_armor_lineups;
        }
        else
        {
            lineups = &armor_lineups;
        }

        lineups->push_back({.equipId = (int)id, .equipType = equip_type_protector});
    }

    for (auto [id, row] :
         from::params::get_param<from::paramdef::EQUIP_PARAM_ACCESSORY_ST>(L"EquipParamAccessory"))
    {
        bool is_dlc = false;
        auto accessory_name = ermerchant::get_message(from::msgbnd::accessory_name, id);
        if (accessory_name.empty())
        {
            is_dlc = true;
            accessory_name = ermerchant::get_message(from::msgbnd::dlc_accessory_name, id);
        }

        if (accessory_name.empty() || accessory_name.starts_with(cut_content_prefix))
        {
            continue;
        }

        row.sellValue = 0;

        std::vector<from::paramdef::SHOP_LINEUP_PARAM> *lineups = nullptr;

        if (is_dlc)
        {
            lineups = &dlc_talisman_lineups;
        }
        else
        {
            lineups = &talisman_lineups;
        }

        lineups->push_back({.equipId = (int)id, .equipType = equip_type_accessory});
    }

    auto replacement_goods_ids = std::set<long long>();
    for (auto [id, row] :
         from::params::get_param<from::paramdef::EQUIP_PARAM_GOODS_ST>(L"EquipParamGoods"))
    {
        if (row.appearanceReplaceItemId != -1)
        {
            replacement_goods_ids.insert(row.appearanceReplaceItemId);
        }
    }

    for (auto [id, row] :
         from::params::get_param<from::paramdef::EQUIP_PARAM_GOODS_ST>(L"EquipParamGoods"))
    {
        // Exclude goods which are obtained automatically in some way
        if (id == goods_memory_of_grace_id || id == goods_phantom_great_rune_id ||
            (id >= goods_flask_begin_id && id < goods_flask_end_id))
        {
            continue;
        }

        // Exclude gestures, which are technically goods but are unlocked in a different way
        if (row.goodsType == goods_type_normal_item && row.sortGroupId == goods_sort_group_gesture)
        {
            continue;
        }

        // Exclude tutorials, which are also goods but aren't useful to buy
        if (row.goodsType == goods_type_info_item && row.sortGroupId == goods_sort_group_tutorial)
        {
            continue;
        }

        // Exclude goods entries that are just used to replace the icon of another entry
        if (replacement_goods_ids.contains(id))
        {
            continue;
        }

        bool is_dlc = false;
        auto goods_name = ermerchant::get_message(from::msgbnd::goods_name, id);
        if (goods_name.empty())
        {
            is_dlc = true;
            goods_name = ermerchant::get_message(from::msgbnd::dlc_goods_name, id);
        }

        if (goods_name.empty() || goods_name == cut_content_prefix)
        {
            continue;
        }

        row.sellValue = 0;

        std::vector<from::paramdef::SHOP_LINEUP_PARAM> *lineups = nullptr;

        if (goods_name.starts_with(cut_content_prefix) || !row.iconId)
        {
            // Put cut items in a separate shop
            lineups = &cut_good_lineups;
        }
        else if (id == goods_golden_seed_id || id == goods_sacred_tear_id)
        {
            // These are classified as materials, but should really appear in the consumables shop
            lineups = is_dlc ? &dlc_consumable_lineups : &consumable_lineups;
        }
        else
        {
            switch (row.goodsType)
            {
            case goods_type_normal_item:
                if (row.isConsume && !row.disable_offline)
                {
                    lineups = is_dlc ? &dlc_consumable_lineups : &consumable_lineups;
                }
                else
                {
                    lineups =
                        is_dlc ? &dlc_miscellaneous_item_lineups : &miscellaneous_item_lineups;
                }
                break;

            case goods_type_sorcery:
            case goods_type_incantation:
            case goods_type_self_buff_sorcery:
            case goods_type_self_buff_incantation:
                lineups = is_dlc ? &dlc_spell_lineups : &spell_lineups;
                break;

            case goods_type_spirit_summon_lesser:
            case goods_type_spirit_summon_greater: {
                // Exclude duplicate entries for upgraded spirit ashes
                auto upgrade_level = id % 100;
                if (upgrade_level == 0)
                {
                    lineups = is_dlc ? &dlc_spirit_summon_lineups : &spirit_summon_lineups;
                }
                break;
            }

            case goods_type_remembrance:
            case goods_type_regenerative_material:
                lineups = is_dlc ? &dlc_consumable_lineups : &consumable_lineups;
                break;

            case goods_type_crafting_material:
            case goods_type_reinforcement_material:
                lineups = is_dlc ? &dlc_material_lineups : &material_lineups;
                break;

            case goods_type_key_item:
            case goods_type_info_item:
            case goods_type_wondrous_physick:
            case goods_type_wondrous_physick_tear:
            case goods_type_great_rune:
                lineups = is_dlc ? &dlc_miscellaneous_item_lineups : &miscellaneous_item_lineups;
                break;
            }
        }

        if (lineups)
        {
            auto event_flag_it = auto_goods_flags.find(id);
            lineups->push_back({
                .equipId = (int)id,
                .eventFlag_forStock =
                    event_flag_it == auto_goods_flags.end() ? 0 : event_flag_it->second,
                .equipType = equip_type_goods,
            });
        }
    }

    for (auto [id, row] :
         from::params::get_param<from::paramdef::EQUIP_PARAM_GEM_ST>(L"EquipParamGem"))
    {
        bool is_dlc = false;
        auto gem_name = ermerchant::get_message(from::msgbnd::gem_name, id);
        if (gem_name.empty())
        {
            is_dlc = true;
            gem_name = ermerchant::get_message(from::msgbnd::dlc_gem_name, id);
        }

        if (gem_name.empty() || gem_name.starts_with(cut_content_prefix))
        {
            continue;
        }

        row.sellValue = 0;

        std::vector<from::paramdef::SHOP_LINEUP_PARAM> *lineups = nullptr;

        if (is_dlc)
        {
            lineups = &dlc_ashes_of_war_lineups;
        }
        else
        {
            lineups = &ash_of_war_lineups;
        }

        lineups->push_back({.equipId = (int)id, .equipType = equip_type_gem});
    }

    // Hook SoloParamRepositoryImp::LookupShopMenu to return the new shops added by the mod
    modutils::hook(
        {
            // Note - the mov instructions are 44 or 45 depending on if this is the Japanese or
            // international .exe, and the stack offset is either -10 or -08. This pattern works
            // for both versions.
            .aob = "?? 8b 4e 14"     // mov r9d, [rsi + 14]
                   "?? 8b 46 10"     // mov r8d, [rsi + 10]
                   "33 d2"           // xor edx, edx
                   "48 8d 4d ??"     // lea rcx, [rbp + ??]
                   "e8 ?? ?? ?? ??", // call SoloParamRepositoryImp::LookupShopMenu
            .offset = 14,
            .relative_offsets = {{1, 5}},
        },
        solo_param_repository_lookup_shop_menu_detour, solo_param_repository_lookup_shop_menu);

    // Hook SoloParamRepositoryImp::LookupShopLineup to return shop lineups for every buyable item
    modutils::hook(
        {
            .aob = "48 8d 15 ?? ?? ?? ??" // lea rdx, [shop_lineup_param_indexes]
                   "45 33 c0"             // xor r8d, r8d
                   "?? ?? ??"             // ???
                   "e8 ?? ?? ?? ??"       // call SoloParamRepositoryImp::GetParamResCap
                   "48 85 c0"             // test rax, rax
                   "74 ??",               // jz end_lbl
            .offset = -129,
        },
        solo_param_repository_lookup_shop_lineup_detour, solo_param_repository_lookup_shop_lineup);

    // Hook OpenRegularShop() to perform some memory hacks when opening up one of the Glorious
    // Merchant shops, in order to change the default sort order. Sorting by item type suits very
    // large lists better.
    modutils::hook(
        {
            .aob = "4c 8b 49 18"           // mov    r9, [rcx + 0x18]
                   "48 8b d9"              // mov    rbx,rcx
                   "48 8d 4c 24 20"        // lea    rcx, [rsp + 0x20]
                   "e8 ?? ?? ?? ??"        // call   OpenRegularShopInner
                   "48 8d 4c 24 20"        // lea    rcx, [rsp + 0x20]
                   "0f 10 00"              // movups xmm0, [rax]
                   "c7 43 10 05 00 00 00", // mov    [rbx + 0x10], 5
            .offset = -6,
        },
        open_regular_shop_detour, open_regular_shop);

    game_data_man_addr = modutils::scan<from::CS::GameDataMan *>({
        .aob = "48 8B 05 ?? ?? ?? ??" // mov rax, [GameDataMan]
               "48 85 C0"             // test rax, rax
               "74 05"                // je 10
               "48 8B 40 58"          // move rax, [rax + 0x58]
               "C3"                   // ret
               "C3",                  // ret
        .relative_offsets = {{3, 7}},
    });
}
