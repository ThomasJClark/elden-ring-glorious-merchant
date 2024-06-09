#pragma once

#include "List.hpp"

namespace from
{
namespace EzState
{

struct StateGroup;
struct State;
struct Call;

class MachineImpl
{
  public:
    virtual ~MachineImpl() = default;

    unsigned char unk1[0x20];
    StateGroup *state_group;
    unsigned char unk2[0x110];
};

struct StateGroup
{
    int id;
    from::list<State> states;
    State *initial_state;
};

struct Transition
{
    State *target_state;
    from::list<Call> pass_commands;
    from::list<Transition *> sub_transitions;
    from::list<unsigned char> evaluator;

    template <size_t evaluator_chars>
    inline Transition(State *target_state, const char (&evaluator_string)[evaluator_chars],
                      from::list<Call> pass_commands = {})
        : target_state(target_state),
          evaluator(reinterpret_cast<unsigned char *>(const_cast<char *>(evaluator_string)),
                    evaluator_chars - 1),
          pass_commands(pass_commands)
    {
    }
};

#pragma pack(push, 1)
struct IntValue
{
    unsigned char start{0x82};
    int value;
    unsigned char end{0xa1};

    inline IntValue(int value) : value(value)
    {
    }
};
#pragma pack(pop)

struct Command
{
    int bank;
    int id;

    bool operator==(Command const &other)
    {
        return bank == other.bank && id == other.id;
    }
};

struct CommandArg
{
    const unsigned char *data;
    unsigned int size;

    template <typename T>
    inline CommandArg(const T &value)
        : data(reinterpret_cast<const unsigned char *>(&value)), size(sizeof(value))
    {
    }
};

struct Call
{
    Command command;
    from::list<CommandArg> args;
};

struct State
{
    int id;
    from::list<Transition *> transitions;
    from::list<Call> entry_commands;
    from::list<Call> exit_commands;
    from::list<Call> while_commands;
};

}
}
