#pragma once

namespace ermerchant
{

static constexpr int shop_capacity = 9999;

namespace shops
{
static constexpr long long weapons = 9100000;
static constexpr long long armor = 9110000;
static constexpr long long spells = 9120000;
static constexpr long long talismans = 9130000;
static constexpr long long ammunition = 9140000;
static constexpr long long ashes_of_war = 9150000;
static constexpr long long spirit_summons = 9160000;
static constexpr long long consumables = 9170000;
static constexpr long long materials = 9180000;
static constexpr long long miscellaneous_items = 9190000;
static constexpr long long cut_goods = 9200000;
static constexpr long long cut_armor = 9210000;
static constexpr long long dlc_weapons = 9300000;
static constexpr long long dlc_armor = 9310000;
static constexpr long long dlc_spells = 9320000;
static constexpr long long dlc_talismans = 9330000;
static constexpr long long dlc_ammunition = 9340000;
static constexpr long long dlc_ashes_of_war = 9350000;
static constexpr long long dlc_spirit_summons = 9360000;
static constexpr long long dlc_consumables = 9370000;
static constexpr long long dlc_materials = 9380000;
static constexpr long long dlc_miscellaneous_items = 9390000;
}

/**
 * Set up new params and hooks used by the Glorious Merchant shop
 */
void setup_shops();

void set_shop_open(bool);

}
