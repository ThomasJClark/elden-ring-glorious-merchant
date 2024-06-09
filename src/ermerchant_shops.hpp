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
}

void setup_shops();

}
