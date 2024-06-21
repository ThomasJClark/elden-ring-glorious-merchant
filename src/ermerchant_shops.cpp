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
#include "from/paramdef/SHOP_LINEUP_PARAM.hpp"

#include "ermerchant_messages.hpp"
#include "from/game_data.hpp"
#include "from/param_lookup.hpp"
#include "from/params.hpp"
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

static const std::map<int, unsigned int> goods_event_flags = {
    {100, 60220},   // Tarnished's Furled Finger
    {101, 60240},   // Duelist's Furled Finger
    {102, 60270},   // Bloody Finger
    {103, 60310},   // Finger Severer
    {104, 60280},   // White Cipher Ring
    {105, 60290},   // Blue Cipher Ring
    {106, 60210},   // Tarnished's Wizened Finger
    {108, 60300},   // Taunter's Tongue
    {109, 60230},   // Small Golden Effigy
    {110, 60250},   // Small Red Effigy
    {130, 60100},   // Spectral Steed Whistle
    {170, 60220},   // Tarnished's Furled Finger
    {171, 60240},   // Duelist's Furled Finger
    {172, 60270},   // Bloody Finger
    {174, 60280},   // White Cipher Ring
    {175, 60290},   // Blue Cipher Ring
    {178, 60300},   // Taunter's Tongue
    {179, 60230},   // Small Golden Effigy
    {180, 60250},   // Small Red Effigy
    {181, 60100},   // Spectral Steed Whistle
    {191, 171},     // Godrick's Great Rune
    {192, 172},     // Radahn's Great Rune
    {193, 173},     // Morgott's Great Rune
    {194, 174},     // Rykard's Great Rune
    {195, 175},     // Mohg's Great Rune
    {196, 176},     // Malenia's Great Rune
    {250, 60020},   // Flask of Wondrous Physick
    {251, 60020},   // Flask of Wondrous Physick
    {10080, 197},   // Great Rune of the Unborn
    {11000, 65000}, // Crimsonspill Crystal Tear
    {11001, 65010}, // Greenspill Crystal Tear
    {11002, 65020}, // Crimson Crystal Tear
    {11003, 65030}, // Crimson Crystal Tear
    {11004, 65040}, // Cerulean Crystal Tear
    {11005, 65050}, // Cerulean Crystal Tear
    {11006, 65060}, // Speckled Hardtear
    {11007, 65070}, // Crimson Bubbletear
    {11008, 65080}, // Opaline Bubbletear
    {11009, 65090}, // Crimsonburst Crystal Tear
    {11010, 65100}, // Greenburst Crystal Tear
    {11011, 65110}, // Opaline Hardtear
    {11012, 65120}, // Winged Crystal Tear
    {11013, 65130}, // Thorny Cracked Tear
    {11014, 65140}, // Spiked Cracked Tear
    {11015, 65150}, // Windy Crystal Tear
    {11016, 65160}, // Ruptured Crystal Tear
    {11017, 65170}, // Ruptured Crystal Tear
    {11018, 65180}, // Leaden Hardtear
    {11019, 65190}, // Twiggy Cracked Tear
    {11020, 65200}, // Crimsonwhorl Bubbletear
    {11021, 65210}, // Strength-knot Crystal Tear
    {11022, 65220}, // Dexterity-knot Crystal Tear
    {11023, 65230}, // Intelligence-knot Crystal Tear
    {11024, 65240}, // Faith-knot Crystal Tear
    {11025, 65250}, // Cerulean Hidden Tear
    {11026, 65260}, // Stonebarb Cracked Tear
    {11027, 65270}, // Purifying Crystal Tear
    {11028, 65280}, // Flame-Shrouding Cracked Tear
    {11029, 65290}, // Magic-Shrouding Cracked Tear
    {11030, 65300}, // Lightning-Shrouding Cracked Tear
    {11031, 65310}, // Holy-Shrouding Cracked Tear
    {8148, 171},    // Godrick's Great Rune-
    {8149, 172},    // Radahn's Great Rune
    {8150, 173},    // Morgott's Great Rune
    {8151, 174},    // Rykard's Great Rune
    {8152, 175},    // Mohg's Great Rune
    {8153, 176},    // Malenia's Great Rune
    {8158, 60110},  // Spirit Calling Bell
    {8163, 60140},  // Tailoring Tools
    {8182, 9500},   // Mending Rune of Perfect Order
    {8183, 9502},   // Mending Rune of the Death-Prince
    {8184, 9504},   // Mending Rune of the Fell Curse
    {8188, 60150},  // Golden Tailoring Tools
    {8500, 60120},  // Crafting Kit
    {8590, 60130},  // Whetstone Knife
    {8600, 62010},  // Map: Limgrave, West
    {8601, 62011},  // Map: Weeping Peninsula
    {8602, 62012},  // Map: Limgrave, East
    {8603, 62020},  // Map: Liurnia, East
    {8604, 62021},  // Map: Liurnia, North
    {8605, 62022},  // Map: Liurnia, West
    {8606, 62030},  // Map: Altus Plateau
    {8607, 62031},  // Map: Leyndell, Royal Capital
    {8608, 62032},  // Map: Mt. Gelmir
    {8609, 62040},  // Map: Caelid
    {8610, 62041},  // Map: Dragonbarrow
    {8611, 62050},  // Map: Mountaintops of the Giants, West
    {8612, 62051},  // Map: Mountaintops of the Giants, East
    {8613, 62060},  // Map: Ainsel River
    {8614, 62061},  // Map: Lake of Rot
    {8615, 62063},  // Map: Siofra River
    {8616, 62062},  // Map: Mohgwyn Palace
    {8617, 62064},  // Map: Deeproot Depths
    {8618, 62052},  // Map: Consecrated Snowfield
    {8970, 65610},  // Iron Whetblade
    {8971, 65640},  // Red-Hot Whetblade
    {8972, 65660},  // Sanctified Whetblade
    {8973, 65680},  // Glintstone Whetblade
    {8974, 65720},  // Black Whetblade
    {9300, 67000},  // Nomadic Warrior's Cookbook [1]
    {9301, 67010},  // Nomadic Warrior's Cookbook [3]
    {9302, 67020},  // Nomadic Warrior's Cookbook [6]
    {9303, 67030},  // Nomadic Warrior's Cookbook [10]
    {9305, 67050},  // Nomadic Warrior's Cookbook [7]
    {9306, 67060},  // Nomadic Warrior's Cookbook [12]
    {9307, 67070},  // Nomadic Warrior's Cookbook [19]
    {9308, 67080},  // Nomadic Warrior's Cookbook [13]
    {9309, 67090},  // Nomadic Warrior's Cookbook [23]
    {9310, 67100},  // Nomadic Warrior's Cookbook [17]
    {9311, 67110},  // Nomadic Warrior's Cookbook [2]
    {9312, 67120},  // Nomadic Warrior's Cookbook [21]
    {9313, 67130},  // Missionary's Cookbook [6]
    {9320, 67200},  // Armorer's Cookbook [1]
    {9321, 67210},  // Armorer's Cookbook [2]
    {9322, 67220},  // Nomadic Warrior's Cookbook [11]
    {9323, 67230},  // Nomadic Warrior's Cookbook [20]
    {9325, 67250},  // Armorer's Cookbook [7]
    {9326, 67260},  // Armorer's Cookbook [4]
    {9327, 67270},  // Nomadic Warrior's Cookbook [18]
    {9328, 67280},  // Armorer's Cookbook [3]
    {9329, 67290},  // Nomadic Warrior's Cookbook [16]
    {9330, 67300},  // Armorer's Cookbook [6]
    {9331, 67310},  // Armorer's Cookbook [5]
    {9340, 67400},  // Glintstone Craftsman's Cookbook [4]
    {9341, 67410},  // Glintstone Craftsman's Cookbook [1]
    {9342, 67420},  // Glintstone Craftsman's Cookbook [5]
    {9343, 67430},  // Nomadic Warrior's Cookbook [9]
    {9344, 67440},  // Glintstone Craftsman's Cookbook [8]
    {9345, 67450},  // Glintstone Craftsman's Cookbook [2]
    {9346, 67460},  // Glintstone Craftsman's Cookbook [6]
    {9347, 67470},  // Glintstone Craftsman's Cookbook [7]
    {9348, 67480},  // Glintstone Craftsman's Cookbook [3]
    {9360, 67600},  // Missionary's Cookbook [2]
    {9361, 67610},  // Missionary's Cookbook [1]
    {9363, 67630},  // Missionary's Cookbook [5]
    {9364, 67640},  // Missionary's Cookbook [4]
    {9365, 67650},  // Missionary's Cookbook [3]
    {9380, 67800},  // Nomadic Warrior's Cookbook [4]
    {9383, 67830},  // Nomadic Warrior's Cookbook [5]
    {9384, 67840},  // Perfumer's Cookbook [1]
    {9385, 67850},  // Perfumer's Cookbook [2]
    {9386, 67860},  // Perfumer's Cookbook [3]
    {9387, 67870},  // Nomadic Warrior's Cookbook [14]
    {9388, 67880},  // Nomadic Warrior's Cookbook [8]
    {9389, 67890},  // Nomadic Warrior's Cookbook [22]
    {9390, 67900},  // Nomadic Warrior's Cookbook [15]
    {9391, 67910},  // Nomadic Warrior's Cookbook [24]
    {9392, 67920},  // Perfumer's Cookbook [4]
    {9400, 68000},  // Ancient Dragon Apostle's Cookbook [1]
    {9401, 68010},  // Ancient Dragon Apostle's Cookbook [2]
    {9402, 68020},  // Ancient Dragon Apostle's Cookbook [4]
    {9403, 68030},  // Ancient Dragon Apostle's Cookbook [3]
    {9420, 68200},  // Fevor's Cookbook [1]
    {9421, 68210},  // Fevor's Cookbook [3]
    {9422, 68220},  // Fevor's Cookbook [2]
    {9423, 68230},  // Missionary's Cookbook [7]
    {9440, 68400},  // Frenzied's Cookbook [1]
    {9441, 68410},  // Frenzied's Cookbook [2]
    {9500, 66000},  // Cracked Pot
    {9501, 66400},  // Ritual Pot
    {9510, 66700},  // Perfume Bottle
};

static from::CS::GameDataMan **game_data_man_addr;

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
    from::params::initialize();

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

            ammunition_lineups.push_back({.equipId = (int)id, .equipType = equip_type_weapon});
        }
        else
        {
            weapon_lineups.push_back({.equipId = (int)id, .equipType = equip_type_weapon});
        }
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

        auto protector_name = get_message(from::msgbnd::protector_name, id);
        if (protector_name.empty() || protector_name == cut_content_prefix)
        {
            continue;
        }

        row.sellValue = 0;

        if (protector_name.starts_with(cut_content_prefix))
        {
            cut_armor_lineups.push_back({.equipId = (int)id, .equipType = equip_type_protector});
        }
        else
        {
            armor_lineups.push_back({.equipId = (int)id, .equipType = equip_type_protector});
        }
    }

    for (auto [id, row] :
         from::params::get_param<from::paramdef::EQUIP_PARAM_ACCESSORY_ST>(L"EquipParamAccessory"))
    {
        auto accessory_name = get_message(from::msgbnd::accessory_name, id);
        if (accessory_name.empty() || accessory_name.starts_with(cut_content_prefix))
        {
            continue;
        }

        row.sellValue = 0;

        talisman_lineups.push_back({.equipId = (int)id, .equipType = equip_type_accessory});
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

        auto goods_name = get_message(from::msgbnd::goods_name, id);
        if (goods_name.empty() || goods_name == cut_content_prefix)
        {
            continue;
        }

        row.sellValue = 0;

        auto event_flag_it = goods_event_flags.find(id);
        auto shop_lineup = from::paramdef::SHOP_LINEUP_PARAM{
            .equipId = (int)id,
            .eventFlag_forStock =
                event_flag_it == goods_event_flags.end() ? 0 : event_flag_it->second,
            .equipType = equip_type_goods};

        // Put cut items in a separate shop
        if (goods_name.starts_with(cut_content_prefix) || !row.iconId)
        {
            cut_good_lineups.push_back(shop_lineup);
            continue;
        }

        // These are classified as materials, but should really appear in the consumables shop
        if (id == goods_golden_seed_id || id == goods_sacred_tear_id)
        {
            consumable_lineups.push_back(shop_lineup);
            continue;
        }

        switch (row.goodsType)
        {
        case goods_type_normal_item:
            if (row.isConsume && !row.disable_offline)
            {
                consumable_lineups.push_back(shop_lineup);
            }
            else
            {
                miscellaneous_item_lineups.push_back(shop_lineup);
            }
            break;

        case goods_type_sorcery:
        case goods_type_incantation:
        case goods_type_self_buff_sorcery:
        case goods_type_self_buff_incantation:
            spell_lineups.push_back(shop_lineup);
            break;

        case goods_type_spirit_summon_lesser:
        case goods_type_spirit_summon_greater: {
            // Exclude duplicate entries for upgraded spirit ashes
            auto upgrade_level = id % 100;
            if (upgrade_level == 0)
            {
                spirit_summon_lineups.push_back(shop_lineup);
            }
            break;
        }

        case goods_type_remembrance:
        case goods_type_regenerative_material:
            consumable_lineups.push_back(shop_lineup);
            break;

        case goods_type_crafting_material:
        case goods_type_reinforcement_material:
            material_lineups.push_back(shop_lineup);
            break;

        case goods_type_key_item:
        case goods_type_info_item:
        case goods_type_wondrous_physick:
        case goods_type_wondrous_physick_tear:
        case goods_type_great_rune:
            miscellaneous_item_lineups.push_back(shop_lineup);
            break;
        }
    }

    for (auto [id, row] :
         from::params::get_param<from::paramdef::EQUIP_PARAM_GEM_ST>(L"EquipParamGem"))
    {
        auto gem_name = get_message(from::msgbnd::gem_name, id);
        if (gem_name.empty() || gem_name.starts_with(cut_content_prefix))
        {
            continue;
        }

        row.sellValue = 0;
        ash_of_war_lineups.push_back({.equipId = (int)id, .equipType = equip_type_gem});
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
