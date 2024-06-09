#include "ermerchant_shops.hpp"

#include <array>
#include <set>
#include <vector>

#include <coresystem/cs_param.hpp>
#include <param/param.hpp>

#include "ermerchant_messages.hpp"
#include "from/param_lookup.hpp"
#include "modutils.hpp"

static const std::wstring cut_content_prefix = L"[ERROR]";

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

struct shop
{
    long long id;
    std::vector<from::paramdef::SHOP_LINEUP_PARAM> lineups;
};

static std::array<shop, 12> mod_shops = {
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

void ermerchant::setup_shops()
{
    from::CS::SoloParamRepositoryImp::wait_for_params(-1);

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

    // Iterate through every obtainable item in the game and create shop lineups in the appropriate
    // ranges
    for (auto [id, row] : from::param::EquipParamWeapon)
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

        // Exclude weapon entries without valid names - these are placeholders for data used by
        // non-weapons (e.g. perfumes) or unused/cut items.
        auto weapon_name = get_message(from::msgbnd::weapon_name, id);
        if (weapon_name.empty() || weapon_name.starts_with(cut_content_prefix))
        {
            continue;
        }

        row.sellValue = 0;

        if (row.wepType == weapon_type_arrow || row.wepType == weapon_type_greatarrow ||
            row.wepType == weapon_type_bolt || row.wepType == weapon_type_ballista_bolt)
        {

            ammunition_lineups.push_back({.equipId = id, .equipType = equip_type_weapon});
        }
        else
        {
            weapon_lineups.push_back({.equipId = id, .equipType = equip_type_weapon});
        }
    }

    for (auto [id, row] : from::param::EquipParamProtector)
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

        auto protector_name = get_message(from::msgbnd::protector_name, id);
        if (protector_name.empty() || protector_name == cut_content_prefix)
        {
            continue;
        }

        row.sellValue = 0;

        if (protector_name.starts_with(cut_content_prefix))
        {
            cut_armor_lineups.push_back({.equipId = id, .equipType = equip_type_protector});
        }
        else
        {
            armor_lineups.push_back({.equipId = id, .equipType = equip_type_protector});
        }
    }

    for (auto [id, row] : from::param::EquipParamAccessory)
    {
        auto accessory_name = get_message(from::msgbnd::accessory_name, id);
        if (accessory_name.empty() || accessory_name.starts_with(cut_content_prefix))
        {
            continue;
        }

        row.sellValue = 0;

        talisman_lineups.push_back({.equipId = id, .equipType = equip_type_accessory});
    }

    auto replacement_goods_ids = std::set<long long>();
    for (auto [id, row] : from::param::EquipParamGoods)
    {
        if (row.appearanceReplaceItemId != -1)
        {
            replacement_goods_ids.insert(row.appearanceReplaceItemId);
        }
    }

    for (auto [id, row] : from::param::EquipParamGoods)
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

        auto goods_name = get_message(from::msgbnd::goods_name, id);
        if (goods_name.empty() || goods_name == cut_content_prefix)
        {
            continue;
        }

        row.sellValue = 0;

        // Put cut items in a separate shop
        if (goods_name.starts_with(cut_content_prefix) || !row.iconId)
        {
            cut_good_lineups.push_back({.equipId = id, .equipType = equip_type_goods});
            continue;
        }

        // These are classified as materials, but should really appear in the consumables shop
        if (id == goods_golden_seed_id || id == goods_sacred_tear_id)
        {
            consumable_lineups.push_back({.equipId = id, .equipType = equip_type_goods});
            continue;
        }

        switch (row.goodsType)
        {
        case goods_type_normal_item:
            if (row.isConsume && !row.disable_offline)
            {
                consumable_lineups.push_back({.equipId = id, .equipType = equip_type_goods});
            }
            else
            {
                miscellaneous_item_lineups.push_back(
                    {.equipId = id, .equipType = equip_type_goods});
            }
            break;

        case goods_type_sorcery:
        case goods_type_incantation:
        case goods_type_self_buff_sorcery:
        case goods_type_self_buff_incantation:
            spell_lineups.push_back({.equipId = id, .equipType = equip_type_goods});
            break;

        case goods_type_spirit_summon_lesser:
        case goods_type_spirit_summon_greater: {
            // Exclude duplicate entries for upgraded spirit ashes
            auto upgrade_level = id % 100;
            if (upgrade_level == 0)
            {
                spirit_summon_lineups.push_back({.equipId = id, .equipType = equip_type_goods});
            }
            break;
        }

        case goods_type_remembrance:
        case goods_type_regenerative_material:
            consumable_lineups.push_back({.equipId = id, .equipType = equip_type_goods});
            break;

        case goods_type_crafting_material:
        case goods_type_reinforcement_material:
            material_lineups.push_back({.equipId = id, .equipType = equip_type_goods});
            break;

        case goods_type_key_item:
        case goods_type_info_item:
        case goods_type_wondrous_physick:
        case goods_type_wondrous_physick_tear:
        case goods_type_great_rune:
            miscellaneous_item_lineups.push_back({.equipId = id, .equipType = equip_type_goods});
            break;
        }
    }

    for (auto [id, row] : from::param::EquipParamGem)
    {
        auto gem_name = get_message(from::msgbnd::gem_name, id);
        if (gem_name.empty() || gem_name.starts_with(cut_content_prefix))
        {
            continue;
        }

        row.sellValue = 0;
        ash_of_war_lineups.push_back({.equipId = id, .equipType = equip_type_gem});
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
}
