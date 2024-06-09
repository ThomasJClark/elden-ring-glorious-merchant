#pragma once

#include "ezstate.hpp"

namespace from
{
namespace talk_command
{
constexpr EzState::Command talk_to_player{.bank = 1, .id = 1};
constexpr EzState::Command remove_my_aggro{.bank = 1, .id = 5};
constexpr EzState::Command force_end_talk{.bank = 1, .id = 8};
constexpr EzState::Command clear_talk_progress_data{.bank = 1, .id = 9};
constexpr EzState::Command show_shop_message{.bank = 1, .id = 10};
constexpr EzState::Command set_event_flag{.bank = 1, .id = 11};
constexpr EzState::Command close_shop_message{.bank = 1, .id = 12};
constexpr EzState::Command open_generic_dialog{.bank = 1, .id = 17};
constexpr EzState::Command force_close_generic_dialog{.bank = 1, .id = 18};
constexpr EzState::Command add_talk_list_data{.bank = 1, .id = 19};
constexpr EzState::Command clear_talk_list_data{.bank = 1, .id = 20};
constexpr EzState::Command open_regular_shop{.bank = 1, .id = 22};
constexpr EzState::Command open_enhance_shop{.bank = 1, .id = 24};
constexpr EzState::Command open_magic_equip{.bank = 1, .id = 28};
constexpr EzState::Command open_repository{.bank = 1, .id = 30};
constexpr EzState::Command open_soul{.bank = 1, .id = 31};
constexpr EzState::Command clear_talk_action_state{.bank = 1, .id = 35};
constexpr EzState::Command set_update_distance{.bank = 1, .id = 38};
constexpr EzState::Command clear_player_damage_info{.bank = 1, .id = 39};
constexpr EzState::Command start_bonfire_anim_loop{.bank = 1, .id = 42};
constexpr EzState::Command end_bonfire_kindle_anim_loop{.bank = 1, .id = 43};
constexpr EzState::Command open_sell_shop{.bank = 1, .id = 46};
constexpr EzState::Command change_player_stat{.bank = 1, .id = 47};
constexpr EzState::Command open_equipment_change_of_purpose_shop{.bank = 1, .id = 48};
constexpr EzState::Command combine_menu_flag_and_event_flag{.bank = 1, .id = 49};
constexpr EzState::Command player_equipment_quantity_change{.bank = 1, .id = 52};
constexpr EzState::Command shuffle_rng_seed{.bank = 1, .id = 57};
constexpr EzState::Command set_rng_seed{.bank = 1, .id = 58};
constexpr EzState::Command replace_tool{.bank = 1, .id = 59};
constexpr EzState::Command give_sp_effect_to_player{.bank = 1, .id = 62};
constexpr EzState::Command force_close_menu{.bank = 1, .id = 67};
constexpr EzState::Command set_talk_time{.bank = 1, .id = 68};
constexpr EzState::Command report_conversation_end_to_havok_behavior{.bank = 1, .id = 71};
constexpr EzState::Command open_conversation_choices_menu{.bank = 1, .id = 76};
constexpr EzState::Command stop_event_anim_without_forcing_conversation_end{.bank = 1, .id = 80};
constexpr EzState::Command open_chara_make_menu{.bank = 1, .id = 81};
constexpr EzState::Command open_choose_quantity_dialog{.bank = 1, .id = 82};
constexpr EzState::Command clear_quantity_value_of_choose_quantity_dialog{.bank = 1, .id = 83};
constexpr EzState::Command set_work_value{.bank = 1, .id = 100};
constexpr EzState::Command update_player_respawn_point{.bank = 1, .id = 101};
constexpr EzState::Command set_message_tag_value{.bank = 1, .id = 102};
constexpr EzState::Command turn_character_to_face_entity{.bank = 1, .id = 103};
constexpr EzState::Command award_item_lot{.bank = 1, .id = 104};
constexpr EzState::Command open_estus_allot_menu{.bank = 1, .id = 105};
constexpr EzState::Command estus_allocation_update{.bank = 1, .id = 108};
constexpr EzState::Command bonfire_activation{.bank = 1, .id = 109};
constexpr EzState::Command open_transposition_shop{.bank = 1, .id = 111};
constexpr EzState::Command reallocate_attributes{.bank = 1, .id = 113};
constexpr EzState::Command request_animation{.bank = 1, .id = 123};
constexpr EzState::Command open_physick_menu{.bank = 1, .id = 130};
constexpr EzState::Command acquire_gesture{.bank = 1, .id = 131};
constexpr EzState::Command open_dragon_communion_shop{.bank = 1, .id = 135};
constexpr EzState::Command open_tailoring_shop{.bank = 1, .id = 142};
constexpr EzState::Command open_ash_of_war_shop{.bank = 1, .id = 143};
constexpr EzState::Command open_puppet_shop{.bank = 1, .id = 144};
constexpr EzState::Command open_dupe_shop{.bank = 1, .id = 146};
constexpr EzState::Command set_event_flag_value{.bank = 1, .id = 147};
constexpr EzState::Command open_champions_equipment_sho{.bank = 1, .id = 148};
}
}
