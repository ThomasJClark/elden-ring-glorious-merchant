#pragma once

namespace from
{

namespace CS
{

class MsgRepositoryImp;

}

enum class msgbnd : unsigned int
{
    goods_name = 10,
    weapon_name = 11,
    protector_name = 12,
    accessory_name = 13,
    goods_info = 20,
    accessory_info = 23,
    goods_caption = 24,
    accessory_caption = 27,
    event_text_for_talk = 33,
    gem_name = 35,
    menu_text = 200,
    line_help = 201,
    dialogues = 204,
    dlc_weapon_name = 310,
    dlc_protector_name = 313,
    dlc_accessory_name = 316,
    dlc_goods_name = 319,
    dlc_gem_name = 322,
};

}
