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

int_value_data event_flag_on = make_int_value(0);

int_value_data generic_dialog_shop_message = make_int_value(0);
from::EzState::arg show_generic_dialog_shop_message_arg_list[1] = {
    generic_dialog_shop_message,
};

extern from::EzState::state browse_inventory_state;
extern from::EzState::state browse_inventory_successor_state;
extern from::EzState::state browse_inventory_items_state;
extern from::EzState::state browse_inventory_items_successor_state;
extern from::EzState::state browse_dlc_inventory_state;
extern from::EzState::state browse_dlc_inventory_successor_state;
extern from::EzState::state browse_dlc_inventory_items_state;
extern from::EzState::state browse_dlc_inventory_items_successor_state;
extern from::EzState::state browse_cut_content_state;
extern from::EzState::state browse_cut_content_successor_state;
extern from::EzState::state gestures_state;
extern from::EzState::state gestures_successor_state;
extern from::EzState::state dlc_gestures_state;
extern from::EzState::state dlc_gestures_successor_state;
extern from::EzState::state cut_gestures_state;
extern from::EzState::state cut_gestures_successor_state;

#define ADD_TALK_LIST_DATA_ARGS(name, index, message_id)                                           \
    int_value_data name##_index_value = make_int_value(index);                                     \
    int_value_data name##_message_id_value = make_int_value(message_id);                           \
    int_value_data name##_unk_value = make_int_value(-1);                                          \
    std::array<from::EzState::arg, 3> name##_args = {name##_index_value, name##_message_id_value,  \
                                                     name##_unk_value}

#define UNLOCK_GESTURE_ARGS(name, gesture_id, event_flag_id)                                       \
    int_value_data name##_gesture_id = make_int_value(gesture_id);                                 \
    int_value_data name##_event_flag_id = make_int_value(event_flag_id);                           \
    std::array<from::EzState::arg, 1> name##_unlock_args = {name##_gesture_id};                    \
    std::array<from::EzState::arg, 2> name##_event_flag_args = {name##_event_flag_id, event_flag_on}

#define UNLOCK_GESTURE_ARGS_NO_EVENT(name, gesture_id)                                             \
    int_value_data name##_gesture_id = make_int_value(gesture_id);                                 \
    std::array<from::EzState::arg, 1> name##_unlock_args = {name##_gesture_id};

#define UNLOCK_GESTURE_EVENTS(name)                                                                \
    from::EzState::event(from::talk_command::acquire_gesture, name##_unlock_args),                 \
        from::EzState::event(from::talk_command::set_event_flag, name##_event_flag_args)

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

ADD_TALK_LIST_DATA_ARGS(browse_inventory, 48, ermerchant::event_text_for_talk::browse_inventory);
ADD_TALK_LIST_DATA_ARGS(browse_cut_content, 49,
                        ermerchant::event_text_for_talk::browse_cut_content);
ADD_TALK_LIST_DATA_ARGS(weapons, 50, ermerchant::event_text_for_talk::weapons);
ADD_TALK_LIST_DATA_ARGS(ammunition, 51, ermerchant::event_text_for_talk::ammunition);
ADD_TALK_LIST_DATA_ARGS(spells, 52, ermerchant::event_text_for_talk::spells);
ADD_TALK_LIST_DATA_ARGS(ashes_of_war, 53, ermerchant::event_text_for_talk::ashes_of_war);
ADD_TALK_LIST_DATA_ARGS(armor, 54, ermerchant::event_text_for_talk::armor);
ADD_TALK_LIST_DATA_ARGS(talismans, 55, ermerchant::event_text_for_talk::talismans);
ADD_TALK_LIST_DATA_ARGS(items, 56, ermerchant::event_text_for_talk::items);
ADD_TALK_LIST_DATA_ARGS(dlc, 57, ermerchant::event_text_for_talk::dlc);
ADD_TALK_LIST_DATA_ARGS(gestures, 58, ermerchant::event_text_for_talk::gestures);
ADD_TALK_LIST_DATA_ARGS(goods, 59, ermerchant::event_text_for_talk::goods);
ADD_TALK_LIST_DATA_ARGS(consumables, 60, ermerchant::event_text_for_talk::consumables);
ADD_TALK_LIST_DATA_ARGS(materials, 61, ermerchant::event_text_for_talk::materials);
ADD_TALK_LIST_DATA_ARGS(spirit_summons, 62, ermerchant::event_text_for_talk::spirit_summons);
ADD_TALK_LIST_DATA_ARGS(miscellaneous_items, 63,
                        ermerchant::event_text_for_talk::miscellaneous_items);
ADD_TALK_LIST_DATA_ARGS(unlock, 64, ermerchant::event_text_for_talk::unlock);
ADD_TALK_LIST_DATA_ARGS(leave, 99, ermerchant::event_text_for_talk::leave);

static constexpr char browse_inventory_evaluator[] =
    "\x57\x84\x82\x30\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 48
static constexpr char browse_cut_content_evaluator[] =
    "\x57\x84\x82\x31\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 49
static constexpr char weapons_evaluator[] =
    "\x57\x84\x82\x32\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 50
static constexpr char ammunition_evaluator[] =
    "\x57\x84\x82\x33\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 51
static constexpr char spells_evaluator[] =
    "\x57\x84\x82\x34\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 52
static constexpr char ashes_of_war_evaluator[] =
    "\x57\x84\x82\x35\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 53
static constexpr char armor_evaluator[] =
    "\x57\x84\x82\x36\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 54
static constexpr char talismans_evaluator[] =
    "\x57\x84\x82\x37\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 55
static constexpr char items_evaluator[] =
    "\x57\x84\x82\x38\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 56
static constexpr char dlc_evaluator[] =
    "\x57\x84\x82\x39\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 57
static constexpr char gestures_evaluator[] =
    "\x57\x84\x82\x3a\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 58
static constexpr char goods_evaluator[] =
    "\x57\x84\x82\x3b\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 59
static constexpr char consumables_evaluator[] =
    "\x57\x84\x82\x3c\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 60
static constexpr char materials_evaluator[] =
    "\x57\x84\x82\x3d\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 61
static constexpr char spirit_summons_evaluator[] =
    "\x57\x84\x82\x3e\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 62
static constexpr char miscellaneous_items_evaluator[] =
    "\x57\x84\x82\x3f\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 63
static constexpr char unlock_evaluator[] =
    "\x57\x84\x82\x40\x00\x00\x00\x95\xa1"; // GetTalkListEntryResult() == 64
static constexpr char else_evaluator[] = "\x41\xa1";

constexpr char talk_menu_closed_evaluator[] =
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

constexpr char shop_closed_evaluator[] =
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

from::EzState::transition main_menu_return_transition(nullptr, else_evaluator);

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
OPEN_REGULAR_SHOP_STATE(5800, dlc_weapons_shop_state, &browse_dlc_inventory_state,
                        ermerchant::shops::dlc_weapons);
OPEN_REGULAR_SHOP_STATE(5801, dlc_armor_shop_state, &browse_dlc_inventory_state,
                        ermerchant::shops::dlc_armor);
OPEN_REGULAR_SHOP_STATE(5802, dlc_spells_shop_state, &browse_dlc_inventory_state,
                        ermerchant::shops::dlc_spells);
OPEN_REGULAR_SHOP_STATE(5803, dlc_talismans_shop_state, &browse_dlc_inventory_state,
                        ermerchant::shops::dlc_talismans);
OPEN_REGULAR_SHOP_STATE(5804, dlc_ammunition_shop_state, &browse_dlc_inventory_state,
                        ermerchant::shops::dlc_ammunition);
OPEN_REGULAR_SHOP_STATE(5805, dlc_ashes_of_war_shop_state, &browse_dlc_inventory_state,
                        ermerchant::shops::dlc_ashes_of_war);
OPEN_REGULAR_SHOP_STATE(5806, dlc_spirit_summons_shop_state, &browse_dlc_inventory_items_state,
                        ermerchant::shops::dlc_spirit_summons);
OPEN_REGULAR_SHOP_STATE(5807, dlc_consumables_shop_state, &browse_dlc_inventory_items_state,
                        ermerchant::shops::dlc_consumables);
OPEN_REGULAR_SHOP_STATE(5808, dlc_materials_shop_state, &browse_dlc_inventory_items_state,
                        ermerchant::shops::dlc_materials);
OPEN_REGULAR_SHOP_STATE(5809, dlc_miscellaneous_items_shop_state, &browse_dlc_inventory_items_state,
                        ermerchant::shops::dlc_miscellaneous_items);

/*
 * "Browse Inventory" submenu
 */
std::array<from::EzState::event, 13> browse_inventory_events = {
    from::EzState::event{from::talk_command::close_shop_message},
    from::EzState::event{from::talk_command::clear_talk_list_data},
    from::EzState::event{from::talk_command::add_talk_list_data, weapons_args},
    from::EzState::event{from::talk_command::add_talk_list_data, ammunition_args},
    from::EzState::event{from::talk_command::add_talk_list_data, spells_args},
    from::EzState::event{from::talk_command::add_talk_list_data, ashes_of_war_args},
    from::EzState::event{from::talk_command::add_talk_list_data, armor_args},
    from::EzState::event{from::talk_command::add_talk_list_data, talismans_args},
    from::EzState::event{from::talk_command::add_talk_list_data, items_args},
    from::EzState::event{from::talk_command::add_talk_list_data, dlc_args},
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

from::EzState::transition weapons_transition(&weapons_shop_state, weapons_evaluator);
from::EzState::transition ammunition_transition(&ammunition_shop_state, ammunition_evaluator);
from::EzState::transition spells_transition(&spells_shop_state, spells_evaluator);
from::EzState::transition ashes_of_war_transition(&ashes_of_war_shop_state, ashes_of_war_evaluator);
from::EzState::transition armor_transition(&armor_shop_state, armor_evaluator);
from::EzState::transition talismans_transition(&talismans_shop_state, talismans_evaluator);
from::EzState::transition items_transition(&browse_inventory_items_state, items_evaluator);
from::EzState::transition dlc_transition(&browse_dlc_inventory_state, dlc_evaluator);
from::EzState::transition gestures_transition(&gestures_state, gestures_evaluator);

std::array<from::EzState::transition *, 10> browse_inventory_successor_transitions = {
    &weapons_transition,  &ammunition_transition,
    &spells_transition,   &ashes_of_war_transition,
    &armor_transition,    &talismans_transition,
    &items_transition,    &dlc_transition,
    &gestures_transition, &main_menu_return_transition,
};

from::EzState::state browse_inventory_successor_state = {
    .id = 5001,
    .transitions = browse_inventory_successor_transitions,
};

/*
 * "Browse Inventory" > "Items" submenu
 */
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

from::EzState::transition consumables_transition(&consumables_shop_state, consumables_evaluator);
from::EzState::transition materials_transition(&materials_shop_state, materials_evaluator);
from::EzState::transition spirit_summons_transition(&spirit_summons_shop_state,
                                                    spirit_summons_evaluator);
from::EzState::transition miscellaneous_items_transition(&miscellaneous_items_shop_state,
                                                         miscellaneous_items_evaluator);
from::EzState::transition items_return_transition(&browse_inventory_state, else_evaluator);
std::array<from::EzState::transition *, 5> browse_inventory_items_successor_transitions = {
    &consumables_transition,         &materials_transition,    &spirit_summons_transition,
    &miscellaneous_items_transition, &items_return_transition,
};
from::EzState::state browse_inventory_items_successor_state = {
    .id = 5003,
    .transitions = browse_inventory_items_successor_transitions,
};

/**
 * "Browse Inventory" > "Gestures" submenu
 */
std::array<from::EzState::event, 5> gestures_events = {
    from::EzState::event{from::talk_command::close_shop_message},
    from::EzState::event{from::talk_command::clear_talk_list_data},
    from::EzState::event{from::talk_command::add_talk_list_data, unlock_args},
    from::EzState::event{from::talk_command::add_talk_list_data, leave_args},
    from::EzState::event{from::talk_command::show_shop_message,
                         show_generic_dialog_shop_message_arg_list},
};
from::EzState::transition gestures_next_transition(&gestures_successor_state,
                                                   talk_menu_closed_evaluator);
std::array<from::EzState::transition *, 1> gestures_transitions = {&gestures_next_transition};
from::EzState::state gestures_state = {
    .id = 5300,
    .transitions = gestures_transitions,
    .entry_events = gestures_events,
};

UNLOCK_GESTURE_ARGS(gesture0, 0, 60800);     // Bow
UNLOCK_GESTURE_ARGS(gesture1, 1, 60801);     // Polite Bow
UNLOCK_GESTURE_ARGS(gesture2, 2, 60802);     // My Thanks
UNLOCK_GESTURE_ARGS(gesture3, 3, 60803);     // Curtsy
UNLOCK_GESTURE_ARGS(gesture4, 4, 60804);     // Reverential Bow
UNLOCK_GESTURE_ARGS(gesture5, 5, 60805);     // My Lord
UNLOCK_GESTURE_ARGS(gesture6, 6, 60806);     // Warm Welcome
UNLOCK_GESTURE_ARGS(gesture7, 7, 60807);     // Wave
UNLOCK_GESTURE_ARGS(gesture8, 8, 60808);     // Casual Greeting
UNLOCK_GESTURE_ARGS(gesture9, 9, 60809);     // Strength!
UNLOCK_GESTURE_ARGS(gesture10, 10, 60810);   // As You Wish
UNLOCK_GESTURE_ARGS(gesture20, 20, 60811);   // Point Forwards
UNLOCK_GESTURE_ARGS(gesture21, 21, 60812);   // Point Upwards
UNLOCK_GESTURE_ARGS(gesture22, 22, 60813);   // Point Downwards
UNLOCK_GESTURE_ARGS(gesture23, 23, 60814);   // Beckon
UNLOCK_GESTURE_ARGS(gesture24, 24, 60815);   // Wait!
UNLOCK_GESTURE_ARGS(gesture25, 25, 60816);   // Calm Down!
UNLOCK_GESTURE_ARGS(gesture30, 30, 60817);   // Nod In Thought
UNLOCK_GESTURE_ARGS(gesture40, 40, 60818);   // Extreme Repentance
UNLOCK_GESTURE_ARGS(gesture41, 41, 60819);   // Grovel For Mercy
UNLOCK_GESTURE_ARGS(gesture50, 50, 60820);   // Rallying Cry
UNLOCK_GESTURE_ARGS(gesture51, 51, 60821);   // Heartening Cry
UNLOCK_GESTURE_ARGS(gesture52, 52, 60822);   // By My Sword
UNLOCK_GESTURE_ARGS(gesture53, 53, 60823);   // Hoslow's Oath
UNLOCK_GESTURE_ARGS(gesture54, 54, 60824);   // Fire Spur Me
UNLOCK_GESTURE_ARGS(gesture60, 60, 60826);   // Bravo!
UNLOCK_GESTURE_ARGS(gesture70, 70, 60827);   // Jump for Joy
UNLOCK_GESTURE_ARGS(gesture71, 71, 60828);   // Triumphant Delight
UNLOCK_GESTURE_ARGS(gesture72, 72, 60829);   // Fancy Spin
UNLOCK_GESTURE_ARGS(gesture73, 73, 60830);   // Finger Snap
UNLOCK_GESTURE_ARGS(gesture80, 80, 60831);   // Dejection
UNLOCK_GESTURE_ARGS(gesture90, 90, 60832);   // Patches' Crouch
UNLOCK_GESTURE_ARGS(gesture91, 91, 60833);   // Crossed Legs
UNLOCK_GESTURE_ARGS(gesture92, 92, 60834);   // Rest
UNLOCK_GESTURE_ARGS(gesture93, 93, 60835);   // Sitting Sideways
UNLOCK_GESTURE_ARGS(gesture94, 94, 60836);   // Dozing Cross-Legged
UNLOCK_GESTURE_ARGS(gesture95, 95, 60837);   // Spread Out
UNLOCK_GESTURE_ARGS(gesture97, 97, 60839);   // Balled Up
UNLOCK_GESTURE_ARGS(gesture98, 98, 60840);   // What Do You Want?
UNLOCK_GESTURE_ARGS(gesture100, 100, 60841); // Prayer
UNLOCK_GESTURE_ARGS(gesture101, 101, 60842); // Desperate Prayer
UNLOCK_GESTURE_ARGS(gesture102, 102, 60843); // Rapture
UNLOCK_GESTURE_ARGS(gesture103, 103, 60845); // Erudition
UNLOCK_GESTURE_ARGS(gesture104, 104, 60846); // Outer Order
UNLOCK_GESTURE_ARGS(gesture105, 105, 60847); // Inner Order
UNLOCK_GESTURE_ARGS(gesture106, 106, 60848); // Golden Order Totality
UNLOCK_GESTURE_ARGS(gesture108, 108, 60849); // The Ring

std::array<from::EzState::event, 92> gestures_unlock_events = {
    UNLOCK_GESTURE_EVENTS(gesture0),   UNLOCK_GESTURE_EVENTS(gesture1),
    UNLOCK_GESTURE_EVENTS(gesture2),   UNLOCK_GESTURE_EVENTS(gesture3),
    UNLOCK_GESTURE_EVENTS(gesture4),   UNLOCK_GESTURE_EVENTS(gesture5),
    UNLOCK_GESTURE_EVENTS(gesture6),   UNLOCK_GESTURE_EVENTS(gesture7),
    UNLOCK_GESTURE_EVENTS(gesture8),   UNLOCK_GESTURE_EVENTS(gesture9),
    UNLOCK_GESTURE_EVENTS(gesture10),  UNLOCK_GESTURE_EVENTS(gesture20),
    UNLOCK_GESTURE_EVENTS(gesture21),  UNLOCK_GESTURE_EVENTS(gesture22),
    UNLOCK_GESTURE_EVENTS(gesture23),  UNLOCK_GESTURE_EVENTS(gesture24),
    UNLOCK_GESTURE_EVENTS(gesture25),  UNLOCK_GESTURE_EVENTS(gesture30),
    UNLOCK_GESTURE_EVENTS(gesture40),  UNLOCK_GESTURE_EVENTS(gesture41),
    UNLOCK_GESTURE_EVENTS(gesture50),  UNLOCK_GESTURE_EVENTS(gesture51),
    UNLOCK_GESTURE_EVENTS(gesture52),  UNLOCK_GESTURE_EVENTS(gesture53),
    UNLOCK_GESTURE_EVENTS(gesture54),  UNLOCK_GESTURE_EVENTS(gesture60),
    UNLOCK_GESTURE_EVENTS(gesture70),  UNLOCK_GESTURE_EVENTS(gesture71),
    UNLOCK_GESTURE_EVENTS(gesture72),  UNLOCK_GESTURE_EVENTS(gesture73),
    UNLOCK_GESTURE_EVENTS(gesture80),  UNLOCK_GESTURE_EVENTS(gesture90),
    UNLOCK_GESTURE_EVENTS(gesture91),  UNLOCK_GESTURE_EVENTS(gesture92),
    UNLOCK_GESTURE_EVENTS(gesture93),  UNLOCK_GESTURE_EVENTS(gesture94),
    UNLOCK_GESTURE_EVENTS(gesture95),  UNLOCK_GESTURE_EVENTS(gesture97),
    UNLOCK_GESTURE_EVENTS(gesture98),  UNLOCK_GESTURE_EVENTS(gesture100),
    UNLOCK_GESTURE_EVENTS(gesture101), UNLOCK_GESTURE_EVENTS(gesture102),
    UNLOCK_GESTURE_EVENTS(gesture103), UNLOCK_GESTURE_EVENTS(gesture104),
    UNLOCK_GESTURE_EVENTS(gesture105), UNLOCK_GESTURE_EVENTS(gesture106),
    // TODO: Adding The Ring while owning it from pre-order causes the gesture acquired popup to
    // appear on load for some reason.
    // UNLOCK_GESTURE_EVENTS(gesture108),
};
from::EzState::transition gestures_unlock_return_transition(&gestures_state, else_evaluator);
std::array<from::EzState::transition *, 1> gestures_unlock_return_transitions = {
    &gestures_unlock_return_transition,
};
from::EzState::state gestures_unlock_state = {
    .id = 5302,
    .transitions = gestures_unlock_return_transitions,
    .entry_events = gestures_unlock_events,
};

from::EzState::transition gestures_unlock_transition(&gestures_unlock_state, unlock_evaluator);
from::EzState::transition gestures_return_transition(&browse_inventory_state, else_evaluator);
std::array<from::EzState::transition *, 2> gestures_successor_transitions = {
    &gestures_unlock_transition,
    &gestures_return_transition,
};
from::EzState::state gestures_successor_state = {
    .id = 5301,
    .transitions = gestures_successor_transitions,
};

/*
 * "Browse DLC Inventory" submenu
 */
std::array<from::EzState::event, 12> browse_dlc_inventory_events = {
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
from::EzState::transition browse_dlc_inventory_next_transition(
    &browse_dlc_inventory_successor_state, talk_menu_closed_evaluator);
std::array<from::EzState::transition *, 1> browse_dlc_inventory_transitions = {
    &browse_dlc_inventory_next_transition};
from::EzState::state browse_dlc_inventory_state = {
    .id = 5700,
    .transitions = browse_dlc_inventory_transitions,
    .entry_events = browse_dlc_inventory_events,
};

from::EzState::transition dlc_weapons_transition(&dlc_weapons_shop_state, weapons_evaluator);
from::EzState::transition dlc_ammunition_transition(&dlc_ammunition_shop_state,
                                                    ammunition_evaluator);
from::EzState::transition dlc_spells_transition(&dlc_spells_shop_state, spells_evaluator);
from::EzState::transition dlc_ashes_of_war_transition(&dlc_ashes_of_war_shop_state,
                                                      ashes_of_war_evaluator);
from::EzState::transition dlc_armor_transition(&dlc_armor_shop_state, armor_evaluator);
from::EzState::transition dlc_talismans_transition(&dlc_talismans_shop_state, talismans_evaluator);
from::EzState::transition dlc_items_transition(&browse_dlc_inventory_items_state, items_evaluator);
from::EzState::transition dlc_gestures_transition(&dlc_gestures_state, gestures_evaluator);
from::EzState::transition dlc_return_to_browse_inventory_transition(&browse_inventory_state,
                                                                    else_evaluator);

std::array<from::EzState::transition *, 10> browse_dlc_inventory_successor_transitions = {
    &dlc_weapons_transition,
    &dlc_ammunition_transition,
    &dlc_spells_transition,
    &dlc_ashes_of_war_transition,
    &dlc_armor_transition,
    &dlc_talismans_transition,
    &dlc_items_transition,
    &dlc_gestures_transition,
    &dlc_return_to_browse_inventory_transition,
};

from::EzState::state browse_dlc_inventory_successor_state = {
    .id = 5701,
    .transitions = browse_dlc_inventory_successor_transitions,
};

/*
 * "Browse Inventory" > "DLC Items" > "Gestures" submenu
 */
std::array<from::EzState::event, 5> dlc_gestures_events = {
    from::EzState::event{from::talk_command::close_shop_message},
    from::EzState::event{from::talk_command::clear_talk_list_data},
    from::EzState::event{from::talk_command::add_talk_list_data, unlock_args},
    from::EzState::event{from::talk_command::add_talk_list_data, leave_args},
    from::EzState::event{from::talk_command::show_shop_message,
                         show_generic_dialog_shop_message_arg_list},
};
from::EzState::transition dlc_gestures_next_transition(&dlc_gestures_successor_state,
                                                       talk_menu_closed_evaluator);
std::array<from::EzState::transition *, 1> dlc_gestures_transitions = {
    &dlc_gestures_next_transition};
from::EzState::state dlc_gestures_state = {
    .id = 5500,
    .transitions = dlc_gestures_transitions,
    .entry_events = dlc_gestures_events,
};

// TODO don't have the events for these, so these will still be in the world. I think this is
// harmless.
UNLOCK_GESTURE_ARGS_NO_EVENT(gesture111, 111); // May the Best Win
UNLOCK_GESTURE_ARGS_NO_EVENT(gesture112, 112); // The Two Fingers
UNLOCK_GESTURE_ARGS_NO_EVENT(gesture114, 114); // Let Us Go Together
UNLOCK_GESTURE_ARGS_NO_EVENT(gesture115, 115); // O Mother

std::array<from::EzState::event, 4> dlc_gestures_unlock_events = {
    from::EzState::event(from::talk_command::acquire_gesture, gesture111_unlock_args),
    from::EzState::event(from::talk_command::acquire_gesture, gesture112_unlock_args),
    from::EzState::event(from::talk_command::acquire_gesture, gesture114_unlock_args),
    from::EzState::event(from::talk_command::acquire_gesture, gesture115_unlock_args)};
from::EzState::transition dlc_gestures_unlock_return_transition(&dlc_gestures_state,
                                                                else_evaluator);
std::array<from::EzState::transition *, 1> dlc_gestures_unlock_return_transitions = {
    &dlc_gestures_unlock_return_transition,
};
from::EzState::state dlc_gestures_unlock_state = {
    .id = 5502,
    .transitions = dlc_gestures_unlock_return_transitions,
    .entry_events = dlc_gestures_unlock_events,
};

from::EzState::transition dlc_gestures_unlock_transition(&dlc_gestures_unlock_state,
                                                         unlock_evaluator);
from::EzState::transition dlc_gestures_return_transition(&browse_dlc_inventory_state,
                                                         else_evaluator);
std::array<from::EzState::transition *, 2> dlc_gestures_successor_transitions = {
    &dlc_gestures_unlock_transition,
    &dlc_gestures_return_transition,
};
from::EzState::state dlc_gestures_successor_state = {
    .id = 5501,
    .transitions = dlc_gestures_successor_transitions,
};

/*
 * "Browse Inventory" > "DLC Items" > "Items" submenu
 */
std::array<from::EzState::event, 8> browse_dlc_inventory_items_events = {
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
from::EzState::transition browse_dlc_inventory_items_next_transition(
    &browse_dlc_inventory_items_successor_state, talk_menu_closed_evaluator);
std::array<from::EzState::transition *, 1> browse_dlc_inventory_items_transitions = {
    &browse_dlc_inventory_items_next_transition};
from::EzState::state browse_dlc_inventory_items_state = {
    .id = 5702,
    .transitions = browse_dlc_inventory_items_transitions,
    .entry_events = browse_dlc_inventory_items_events,
};

from::EzState::transition dlc_consumables_transition(&dlc_consumables_shop_state,
                                                     consumables_evaluator);
from::EzState::transition dlc_materials_transition(&dlc_materials_shop_state, materials_evaluator);
from::EzState::transition dlc_spirit_summons_transition(&dlc_spirit_summons_shop_state,
                                                        spirit_summons_evaluator);
from::EzState::transition dlc_miscellaneous_items_transition(&dlc_miscellaneous_items_shop_state,
                                                             miscellaneous_items_evaluator);
from::EzState::transition dlc_items_return_transition(&browse_dlc_inventory_state, else_evaluator);
std::array<from::EzState::transition *, 5> browse_dlc_inventory_items_successor_transitions = {
    &dlc_consumables_transition,    &dlc_materials_transition,
    &dlc_spirit_summons_transition, &dlc_miscellaneous_items_transition,
    &dlc_items_return_transition,
};
from::EzState::state browse_dlc_inventory_items_successor_state = {
    .id = 5703,
    .transitions = browse_dlc_inventory_items_successor_transitions,
};

/*
 * "Browse Cut Content" submenu
 */
std::array<from::EzState::event, 7> browse_cut_content_events = {
    from::EzState::event{from::talk_command::close_shop_message},
    from::EzState::event{from::talk_command::clear_talk_list_data},
    from::EzState::event{from::talk_command::add_talk_list_data, armor_args},
    from::EzState::event{from::talk_command::add_talk_list_data, goods_args},
    from::EzState::event{from::talk_command::add_talk_list_data, gestures_args},
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

from::EzState::transition cut_armor_transition(&cut_armor_shop_state, armor_evaluator);
from::EzState::transition cut_goods_transition(&cut_goods_shop_state, goods_evaluator);
from::EzState::transition cut_gestures_transition(&cut_gestures_state, gestures_evaluator);
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
 * "Browse Cut Content" > "Gestures" submenu
 */
std::array<from::EzState::event, 5> cut_gestures_events = {
    from::EzState::event{from::talk_command::close_shop_message},
    from::EzState::event{from::talk_command::clear_talk_list_data},
    from::EzState::event{from::talk_command::add_talk_list_data, unlock_args},
    from::EzState::event{from::talk_command::add_talk_list_data, leave_args},
    from::EzState::event{from::talk_command::show_shop_message,
                         show_generic_dialog_shop_message_arg_list},
};
from::EzState::transition cut_gestures_next_transition(&cut_gestures_successor_state,
                                                       talk_menu_closed_evaluator);
std::array<from::EzState::transition *, 1> cut_gestures_transitions = {
    &cut_gestures_next_transition};
from::EzState::state cut_gestures_state = {
    .id = 5400,
    .transitions = cut_gestures_transitions,
    .entry_events = cut_gestures_events,
};

UNLOCK_GESTURE_ARGS(gesture55, 55, 60825); // The Carian Oath
UNLOCK_GESTURE_ARGS(gesture96, 96, 60838); // Fetal Position

std::array<from::EzState::event, 4> cut_gestures_unlock_events = {UNLOCK_GESTURE_EVENTS(gesture55),
                                                                  UNLOCK_GESTURE_EVENTS(gesture96)};
from::EzState::transition cut_gestures_unlock_return_transition(&cut_gestures_state,
                                                                else_evaluator);
std::array<from::EzState::transition *, 1> cut_gestures_unlock_return_transitions = {
    &cut_gestures_unlock_return_transition,
};
from::EzState::state cut_gestures_unlock_state = {
    .id = 5402,
    .transitions = cut_gestures_unlock_return_transitions,
    .entry_events = cut_gestures_unlock_events,
};

from::EzState::transition cut_gestures_unlock_transition(&cut_gestures_unlock_state,
                                                         unlock_evaluator);
from::EzState::transition cut_gestures_return_transition(&browse_cut_content_state, else_evaluator);
std::array<from::EzState::transition *, 2> cut_gestures_successor_transitions = {
    &cut_gestures_unlock_transition,
    &cut_gestures_return_transition,
};
from::EzState::state cut_gestures_successor_state = {
    .id = 5401,
    .transitions = cut_gestures_successor_transitions,
};

/**
 * Main menu
 */
from::EzState::transition browse_inventory_transition(&browse_inventory_state,
                                                      browse_inventory_evaluator);
from::EzState::transition browse_cut_content_transition(&browse_cut_content_state,
                                                        browse_cut_content_evaluator);

#undef ADD_TALK_LIST_DATA_ARGS

};
