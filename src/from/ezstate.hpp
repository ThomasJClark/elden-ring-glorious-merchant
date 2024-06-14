#pragma once

#include <span>

namespace from
{
namespace EzState
{

struct state;
struct state_group;
struct event;

namespace detail
{

class EzStateMachineImpl
{
  public:
    virtual ~EzStateMachineImpl() = default;

    unsigned char unk1[0x20];
    state_group *state_group;
    unsigned char unk2[0x110];
};

}

struct state_group
{
    int id;
    std::span<state> states;
    state *initial_state;
};

struct transition
{
    state *target_state;
    std::span<event> pass_events;
    std::span<transition *> sub_transitions;
    std::span<unsigned char> evaluator;

    template <size_t evaluator_chars>
    inline transition(state *target_state, const char (&evaluator_string)[evaluator_chars],
                      std::span<event> pass_events = {})
        : target_state(target_state),
          evaluator(reinterpret_cast<unsigned char *>(const_cast<char *>(evaluator_string)),
                    evaluator_chars - 1),
          pass_events(pass_events)
    {
    }
};

struct command
{
    int bank;
    int id;

    bool operator==(command const &other)
    {
        return bank == other.bank && id == other.id;
    }
};

typedef std::span<unsigned char> arg;

struct event
{
    command command;
    std::span<arg> args;
};

struct state
{
    int id;
    std::span<transition *> transitions;
    std::span<event> entry_events;
    std::span<event> exit_events;
    std::span<event> while_events;
};

}
}
