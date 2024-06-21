#pragma once

#include "paramdef/SHOP_LINEUP_PARAM.hpp"

namespace from
{

struct find_shop_menu_result
{
    unsigned char shop_type;
    int id;
    const from::paramdef::SHOP_LINEUP_PARAM *row;
};

struct find_shop_lineup_result
{
    unsigned char shop_type;
    int id;
    const from::paramdef::SHOP_LINEUP_PARAM *row;
};

}