/**
 * ermerchant_talkscript.cpp
 *
 * Talkscript patching hook. This intercepts the start of Kalé's dialogue tree, and patches it to
 * include more options for modded shops.
 */
#include "ermerchant_talkscript.hpp"

#include <algorithm>
#include <array>
#include <span>
#include <spdlog/spdlog.h>

#include "ermerchant_messages.hpp"
#include "ermerchant_shops.hpp"
#include "ermerchant_talkscript_utils.hpp"
#include "from/ezstate.hpp"
#include "from/talk_commands.hpp"
#include "modutils.hpp"

/**
 * Returns true if the given state is a GetTalkListEntryResult() switch that, in at least one case,
 * transitions to the sell shop. We use this to identify where to patch the transition into the
 * modded talk menus.
 */
bool is_menu_transition_state(from::EzState::state &state)
{
    for (auto &transition : state.transitions)
    {
        auto target_state = transition->target_state;
        if (!target_state)
        {
            continue;
        }

        if (target_state->entry_events.size() > 0 &&
            target_state->entry_events[0].command == from::talk_command::open_sell_shop)
        {
            return true;
        }

        auto transitions = target_state->transitions;
        if (transitions.size() == 0)
        {
            continue;
        }

        auto next_target_state = transitions[0]->target_state;
        if (!next_target_state)
        {
            continue;
        }

        if (next_target_state->entry_events.size() > 0 &&
            next_target_state->entry_events[0].command == from::talk_command::open_sell_shop)
        {
            return true;
        }
    }

    return false;
}

static std::array<from::EzState::transition *, 100> patched_transition_array;

/**
 * Check if the given state group is the main menu for a merchant, and patch it to contain the
 * modded menu options
 */
static bool patch_states(from::EzState::state_group *state_group)
{
    from::EzState::event *add_menu1_event = nullptr;
    from::EzState::event *add_menu2_event = nullptr;
    from::EzState::state *menu_transition_state = nullptr;

    for (auto &state : state_group->states)
    {
        // Look for "Purchase"/"Sell" menu items indicating the main menu for a merchant
        for (auto &event : state.entry_events)
        {
            if (event.command == from::talk_command::add_talk_list_data)
            {
                auto message_id = get_int_value(event.args[1]);

                if (message_id == ermerchant::event_text_for_talk::purchase)
                {
                    add_menu1_event = &event;
                }
                else if (message_id == ermerchant::event_text_for_talk::sell)
                {
                    add_menu2_event = &event;
                }
                else if (message_id == ermerchant::event_text_for_talk::browse_inventory ||
                         message_id == ermerchant::event_text_for_talk::browse_cut_content)
                {
                    spdlog::debug("Not patching state group x{}, already patched",
                                  0x7fffffff - state_group->id);
                    return true;
                }
            }
        }

        // Look for the state where we transition to the chosen menu item
        if (is_menu_transition_state(state))
        {
            menu_transition_state = &state;
        }
    }

    if (!add_menu1_event || !add_menu2_event || !menu_transition_state)
    {
        return false;
    }

    spdlog::info("Patching state group x{}", 0x7fffffff - state_group->id);

    // Change the "Purchase"/"Sell" menu options to "Browse Inventory"/"Browse Cut Content"
    add_menu1_event->args[0] = browse_inventory_index_value;
    add_menu1_event->args[1] = browse_inventory_message_id_value;
    add_menu2_event->args[0] = browse_cut_content_index_value;
    add_menu2_event->args[1] = browse_cut_content_message_id_value;

    // Add transitions to handle the new menu options. Note: they're added as the second and third
    // last elif statements, because the last one is an else that closes the talk menu.
    auto &transitions = menu_transition_state->transitions;

    std::copy(transitions.begin(), transitions.end() - 1, patched_transition_array.begin());
    auto start_index = transitions.size() - 1;
    patched_transition_array[start_index] = &browse_inventory_transition;
    patched_transition_array[start_index + 1] = &browse_cut_content_transition;
    patched_transition_array[start_index + 2] = transitions.back();

    menu_transition_state->transitions = {patched_transition_array.data(), transitions.size() + 2};

    return true;
}

static void (*ezstate_enter_state)(from::EzState::state *,
                                   from::EzState::detail::EzStateMachineImpl *, void *);

/**
 * Hook for EzState::state::Enter()
 *
 * Patches merchant dialogue trees to open the shops added by this mod
 */
static void ezstate_enter_state_detour(from::EzState::state *state,
                                       from::EzState::detail::EzStateMachineImpl *machine,
                                       void *unk)
{
    if (state == machine->state_group->initial_state)
    {
        if (patch_states(machine->state_group))
        {
            main_menu_return_transition.target_state = state;
        }
    }

    ezstate_enter_state(state, machine, unk);
}

void ermerchant::setup_talkscript()
{
    modutils::hook(
        {
            .aob = "80 7e 18 00"     // cmp byte ptr [rsi+0x18], 0
                   "74 15"           // je 27
                   "4c 8d 44 24 40"  // lea r8, [rsp+0x40]
                   "48 8b d6"        // mov rdx, rsi
                   "48 8b 4e 20"     // mov rcx, qword ptr [rsi+0x20]
                   "e8 ?? ?? ?? ??", // call EzState::state::Enter
            .offset = 18,
            .relative_offsets = {{1, 5}},
        },
        ezstate_enter_state_detour, ezstate_enter_state);
}
