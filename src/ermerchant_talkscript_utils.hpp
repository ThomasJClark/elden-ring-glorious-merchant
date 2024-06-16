/**
 * ermerchant_talkscript_utils.hpp
 *
 * Repetitive/tedious data structure setup for modded talkscript states used by
 * ermerchant_talkscript.cpp. This sets up a few new states for submenus to open various shops
 * and acquire gestures, which are then patched into Kalé's vanilla menu.
 */
#include <array>

#include "from/ezstate.hpp"
#include "from/talk_commands.hpp"

#include "ermerchant_messages.hpp"

namespace
{

typedef std::array<unsigned char, 6> int_value_data;

/**
 * Create an ESD expression representing a 4 byte integer
 */
constexpr int_value_data make_int_value(int value)
{
    return {
        0x82,
        static_cast<unsigned char>((value & 0x000000ff)),
        static_cast<unsigned char>((value & 0x0000ff00) >> 8),
        static_cast<unsigned char>((value & 0x00ff0000) >> 16),
        static_cast<unsigned char>((value & 0xff000000) >> 24),
        0xa1,
    };
}

/**
 * Parse an ESD expression containing only a 1 or 4 byte integer
 */
int get_int_value(from::EzState::arg &arg)
{
    // Single byte (plus final 0xa1) - used to store integers from -64 to 63
    if (arg.size() == 2)
    {
        return arg[0] - 64;
    }

    // Five bytes (plus final 0xa1) - used to store larger integers
    if (arg.size() == 6 && arg[0] == 0x82)
    {
        return *reinterpret_cast<int *>(&arg[1]);
    }

    return -1;
}

int_value_data generic_dialog_shop_message = make_int_value(0);
from::EzState::arg show_generic_dialog_shop_message_arg_list[1] = {
    generic_dialog_shop_message,
};

extern from::EzState::state browse_inventory_state;
extern from::EzState::state browse_inventory_successor_state;
extern from::EzState::state browse_inventory_items_state;
extern from::EzState::state browse_inventory_items_successor_state;
extern from::EzState::state browse_cut_content_state;
extern from::EzState::state browse_cut_content_successor_state;

const char talk_menu_closed_evaluator[40] =
    // CheckSpecificPersonMenuIsOpen(1, 0) == 1
    "\x7b"
    "\x82\x01\x00\x00\x00"
    "\x82\x00\x00\x00\x00"
    "\x86"
    "\x82\x01\x00\x00\x00"
    "\x95"
    // CheckSpecificPersonGenericDialogIsOpen(0) == 0
    "\x7a"
    "\x82\x00\x00\x00\x00"
    "\x85"
    "\x82\x00\x00\x00\x00"
    "\x95"
    // &&
    "\x98"
    // == 0
    "\x82\x00\x00\x00\x00"
    "\x95"
    "\xa1";

const char shop_closed_evaluator[40] =
    // CheckSpecificPersonMenuIsOpen(5, 0) == 1
    "\x7b"
    "\x82\x05\x00\x00\x00"
    "\x82\x00\x00\x00\x00"
    "\x86"
    "\x82\x01\x00\x00\x00"
    "\x95"
    // CheckSpecificPersonGenericDialogIsOpen(0) == 0
    "\x7a"
    "\x82\x00\x00\x00\x00"
    "\x85"
    "\x82\x00\x00\x00\x00"
    "\x95"
    // &&
    "\x98"
    // == 0
    "\x82\x00\x00\x00\x00"
    "\x95"
    "\xa1";

#define ADD_TALK_LIST_DATA_ARGS(name, index, message_id)                                           \
    int_value_data name##_index_value = make_int_value(index);                                     \
    int_value_data name##_message_id_value = make_int_value(message_id);                           \
    int_value_data name##_unk_value = make_int_value(-1);                                          \
    std::array<from::EzState::arg, 3> name##_args = {name##_index_value, name##_message_id_value,  \
                                                     name##_unk_value}

#define OPEN_REGULAR_SHOP_STATE(state_id, name, prev_state, shop_id)                               \
    int_value_data name##_begin_id_value = make_int_value(shop_id);                                \
    int_value_data name##_end_id_value = make_int_value(shop_id + ermerchant::shop_capacity);      \
    std::array<from::EzState::arg, 2> name##_args = {name##_begin_id_value, name##_end_id_value};  \
    std::array<from::EzState::event, 1> name##_events = {                                          \
        from::EzState::event{from::talk_command::open_regular_shop, name##_args},                  \
    };                                                                                             \
    from::EzState::transition name##_transition(prev_state, shop_closed_evaluator);                \
    std::array<from::EzState::transition *, 1> name##_transitions = {                              \
        &name##_transition,                                                                        \
    };                                                                                             \
    from::EzState::state name## = {                                                                \
        .id = state_id,                                                                            \
        .transitions = name##_transitions,                                                         \
        .entry_events = name##_events,                                                             \
    }

ADD_TALK_LIST_DATA_ARGS(leave, 99, ermerchant::event_text_for_talk::leave);

from::EzState::transition main_menu_return_transition(nullptr, "\x41\xa1");

/*
 * Shop states
 */
OPEN_REGULAR_SHOP_STATE(5200, weapons_shop_state, &browse_inventory_state,
                        ermerchant::shops::weapons);
OPEN_REGULAR_SHOP_STATE(5201, armor_shop_state, &browse_inventory_state, ermerchant::shops::armor);
OPEN_REGULAR_SHOP_STATE(5202, spells_shop_state, &browse_inventory_state,
                        ermerchant::shops::spells);
OPEN_REGULAR_SHOP_STATE(5203, talismans_shop_state, &browse_inventory_state,
                        ermerchant::shops::talismans);
OPEN_REGULAR_SHOP_STATE(5204, ammunition_shop_state, &browse_inventory_state,
                        ermerchant::shops::ammunition);
OPEN_REGULAR_SHOP_STATE(5205, ashes_of_war_shop_state, &browse_inventory_state,
                        ermerchant::shops::ashes_of_war);
OPEN_REGULAR_SHOP_STATE(5206, spirit_summons_shop_state, &browse_inventory_items_state,
                        ermerchant::shops::spirit_summons);
OPEN_REGULAR_SHOP_STATE(5207, consumables_shop_state, &browse_inventory_items_state,
                        ermerchant::shops::consumables);
OPEN_REGULAR_SHOP_STATE(5208, materials_shop_state, &browse_inventory_items_state,
                        ermerchant::shops::materials);
OPEN_REGULAR_SHOP_STATE(5209, miscellaneous_items_shop_state, &browse_inventory_items_state,
                        ermerchant::shops::miscellaneous_items);
OPEN_REGULAR_SHOP_STATE(5210, cut_goods_shop_state, &browse_cut_content_state,
                        ermerchant::shops::cut_goods);
OPEN_REGULAR_SHOP_STATE(5211, cut_armor_shop_state, &browse_cut_content_state,
                        ermerchant::shops::cut_armor);

/*
 * "Browse Inventory" submenu
 */
ADD_TALK_LIST_DATA_ARGS(weapons, 50, ermerchant::event_text_for_talk::weapons);
ADD_TALK_LIST_DATA_ARGS(ammunition, 51, ermerchant::event_text_for_talk::ammunition);
ADD_TALK_LIST_DATA_ARGS(spells, 52, ermerchant::event_text_for_talk::spells);
ADD_TALK_LIST_DATA_ARGS(ashes_of_war, 53, ermerchant::event_text_for_talk::ashes_of_war);
ADD_TALK_LIST_DATA_ARGS(armor, 54, ermerchant::event_text_for_talk::armor);
ADD_TALK_LIST_DATA_ARGS(talismans, 55, ermerchant::event_text_for_talk::talismans);
ADD_TALK_LIST_DATA_ARGS(items, 56, ermerchant::event_text_for_talk::items);
ADD_TALK_LIST_DATA_ARGS(gestures, 57, ermerchant::event_text_for_talk::gestures);
std::array<from::EzState::event, 12> browse_inventory_events = {
    from::EzState::event{from::talk_command::close_shop_message},
    from::EzState::event{from::talk_command::clear_talk_list_data},
    from::EzState::event{from::talk_command::add_talk_list_data, weapons_args},
    from::EzState::event{from::talk_command::add_talk_list_data, ammunition_args},
    from::EzState::event{from::talk_command::add_talk_list_data, spells_args},
    from::EzState::event{from::talk_command::add_talk_list_data, ashes_of_war_args},
    from::EzState::event{from::talk_command::add_talk_list_data, armor_args},
    from::EzState::event{from::talk_command::add_talk_list_data, talismans_args},
    from::EzState::event{from::talk_command::add_talk_list_data, items_args},
    from::EzState::event{from::talk_command::add_talk_list_data, gestures_args},
    from::EzState::event{from::talk_command::add_talk_list_data, leave_args},
    from::EzState::event{from::talk_command::show_shop_message,
                         show_generic_dialog_shop_message_arg_list},
};
from::EzState::transition browse_inventory_next_transition(&browse_inventory_successor_state,
                                                           talk_menu_closed_evaluator);
std::array<from::EzState::transition *, 1> browse_inventory_transitions = {
    &browse_inventory_next_transition};
from::EzState::state browse_inventory_state = {
    .id = 5000,
    .transitions = browse_inventory_transitions,
    .entry_events = browse_inventory_events,
};

// if GetTalkListEntryResult() == 50
from::EzState::transition weapons_transition(&weapons_shop_state,
                                             "\x57\x84\x82\x32\x00\x00\x00\x95\xa1");
// if GetTalkListEntryResult() == 51
from::EzState::transition ammunition_transition(&ammunition_shop_state,
                                                "\x57\x84\x82\x33\x00\x00\x00\x95\xa1");
// if GetTalkListEntryResult() == 52
from::EzState::transition spells_transition(&spells_shop_state,
                                            "\x57\x84\x82\x34\x00\x00\x00\x95\xa1");
// if GetTalkListEntryResult() == 53
from::EzState::transition ashes_of_war_transition(&ashes_of_war_shop_state,
                                                  "\x57\x84\x82\x35\x00\x00\x00\x95\xa1");
// if GetTalkListEntryResult() == 54
from::EzState::transition armor_transition(&armor_shop_state,
                                           "\x57\x84\x82\x36\x00\x00\x00\x95\xa1");
// if GetTalkListEntryResult() == 55
from::EzState::transition talismans_transition(&talismans_shop_state,
                                               "\x57\x84\x82\x37\x00\x00\x00\x95\xa1");
// if GetTalkListEntryResult() == 56
from::EzState::transition items_transition(&browse_inventory_items_state,
                                           "\x57\x84\x82\x38\x00\x00\x00\x95\xa1");
// if GetTalkListEntryResult() == 57
from::EzState::transition gestures_transition(nullptr, "\x57\x84\x82\x39\x00\x00\x00\x95\xa1");

std::array<from::EzState::transition *, 10> browse_inventory_successor_transitions = {
    &weapons_transition,      &ammunition_transition, &spells_transition,
    &ashes_of_war_transition, &armor_transition,      &talismans_transition,
    &items_transition,        &gestures_transition,   &main_menu_return_transition,
};

from::EzState::state browse_inventory_successor_state = {
    .id = 5001,
    .transitions = browse_inventory_successor_transitions,
};

/*
 * "Browse Inventory" > "Items" submenu
 */
ADD_TALK_LIST_DATA_ARGS(consumables, 50, ermerchant::event_text_for_talk::consumables);
ADD_TALK_LIST_DATA_ARGS(materials, 51, ermerchant::event_text_for_talk::materials);
ADD_TALK_LIST_DATA_ARGS(spirit_summons, 52, ermerchant::event_text_for_talk::spirit_summons);
ADD_TALK_LIST_DATA_ARGS(miscellaneous_items, 53,
                        ermerchant::event_text_for_talk::miscellaneous_items);
std::array<from::EzState::event, 8> browse_inventory_items_events = {
    from::EzState::event{from::talk_command::close_shop_message},
    from::EzState::event{from::talk_command::clear_talk_list_data},
    from::EzState::event{from::talk_command::add_talk_list_data, consumables_args},
    from::EzState::event{from::talk_command::add_talk_list_data, materials_args},
    from::EzState::event{from::talk_command::add_talk_list_data, spirit_summons_args},
    from::EzState::event{from::talk_command::add_talk_list_data, miscellaneous_items_args},
    from::EzState::event{from::talk_command::add_talk_list_data, leave_args},
    from::EzState::event{from::talk_command::show_shop_message,
                         show_generic_dialog_shop_message_arg_list},
};
from::EzState::transition browse_inventory_items_next_transition(
    &browse_inventory_items_successor_state, talk_menu_closed_evaluator);
std::array<from::EzState::transition *, 1> browse_inventory_items_transitions = {
    &browse_inventory_items_next_transition};
from::EzState::state browse_inventory_items_state = {
    .id = 5002,
    .transitions = browse_inventory_items_transitions,
    .entry_events = browse_inventory_items_events,
};

// if GetTalkListEntryResult() == 50
from::EzState::transition consumables_transition(&consumables_shop_state,
                                                 "\x57\x84\x82\x32\x00\x00\x00\x95\xa1");
// elif GetTalkListEntryResult() == 51
from::EzState::transition materials_transition(&materials_shop_state,
                                               "\x57\x84\x82\x33\x00\x00\x00\x95\xa1");
// elif GetTalkListEntryResult() == 52
from::EzState::transition spirit_summons_transition(&spirit_summons_shop_state,
                                                    "\x57\x84\x82\x34\x00\x00\x00\x95\xa1");
// elif GetTalkListEntryResult() == 53
from::EzState::transition miscellaneous_items_transition(&miscellaneous_items_shop_state,
                                                         "\x57\x84\x82\x35\x00\x00\x00\x95\xa1");
// else
from::EzState::transition items_return_transition(&browse_inventory_state, "\x41\xa1");

std::array<from::EzState::transition *, 5> browse_inventory_items_successor_transitions = {
    &consumables_transition,         &materials_transition,    &spirit_summons_transition,
    &miscellaneous_items_transition, &items_return_transition,
};

from::EzState::state browse_inventory_items_successor_state = {
    .id = 5003,
    .transitions = browse_inventory_items_successor_transitions,
};

/*
 * "Browse Cut Content" submenu
 */
ADD_TALK_LIST_DATA_ARGS(cut_armor, 50, ermerchant::event_text_for_talk::armor);
ADD_TALK_LIST_DATA_ARGS(cut_goods, 51, ermerchant::event_text_for_talk::goods);
ADD_TALK_LIST_DATA_ARGS(cut_gestures, 52, ermerchant::event_text_for_talk::gestures);
std::array<from::EzState::event, 7> browse_cut_content_events = {
    from::EzState::event{from::talk_command::close_shop_message},
    from::EzState::event{from::talk_command::clear_talk_list_data},
    from::EzState::event{from::talk_command::add_talk_list_data, cut_armor_args},
    from::EzState::event{from::talk_command::add_talk_list_data, cut_goods_args},
    from::EzState::event{from::talk_command::add_talk_list_data, cut_gestures_args},
    from::EzState::event{from::talk_command::add_talk_list_data, leave_args},
    from::EzState::event{from::talk_command::show_shop_message,
                         show_generic_dialog_shop_message_arg_list},
};
from::EzState::transition browse_cut_content_next_transition(&browse_cut_content_successor_state,
                                                             talk_menu_closed_evaluator);
std::array<from::EzState::transition *, 1> browse_cut_content_transitions = {
    &browse_cut_content_next_transition};
from::EzState::state browse_cut_content_state = {
    .id = 5100,
    .transitions = browse_cut_content_transitions,
    .entry_events = browse_cut_content_events,
};

// if GetTalkListEntryResult() == 50
from::EzState::transition cut_armor_transition(&cut_armor_shop_state,
                                               "\x57\x84\x82\x32\x00\x00\x00\x95\xa1");
// elif GetTalkListEntryResult() == 51
from::EzState::transition cut_goods_transition(&cut_goods_shop_state,
                                               "\x57\x84\x82\x33\x00\x00\x00\x95\xa1");
// elif GetTalkListEntryResult() == 52
from::EzState::transition cut_gestures_transition(nullptr, "\x57\x84\x82\x34\x00\x00\x00\x95\xa1");

std::array<from::EzState::transition *, 4> browse_cut_content_successor_transitions = {
    &cut_armor_transition,
    &cut_goods_transition,
    &cut_gestures_transition,
    &main_menu_return_transition,
};

from::EzState::state browse_cut_content_successor_state = {
    .id = 5101,
    .transitions = browse_cut_content_successor_transitions,
};

/**
 * Main menu
 */
ADD_TALK_LIST_DATA_ARGS(browse_inventory, 50, ermerchant::event_text_for_talk::browse_inventory);
ADD_TALK_LIST_DATA_ARGS(browse_cut_content, 51,
                        ermerchant::event_text_for_talk::browse_cut_content);

// if GetTalkListEntryResult() == 50
from::EzState::transition browse_inventory_transition(&browse_inventory_state,
                                                      "\x57\x84\x82\x32\x00\x00\x00\x95\xa1");
// elif GetTalkListEntryResult() == 51
from::EzState::transition browse_cut_content_transition(&browse_cut_content_state,
                                                        "\x57\x84\x82\x33\x00\x00\x00\x95\xa1");

#undef ADD_TALK_LIST_DATA_ARGS

};
