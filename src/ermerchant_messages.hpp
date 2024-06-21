#pragma once

#include <map>
#include <string>

#include "from/messages.hpp"

namespace ermerchant
{

namespace event_text_for_talk
{
// New messages added by the mod
static constexpr int weapons = 99999000;
static constexpr int armor = 99999001;
static constexpr int spells = 99999002;
static constexpr int talismans = 99999003;
static constexpr int ammunition = 99999004;
static constexpr int ashes_of_war = 99999005;
static constexpr int consumables = 99999006;
static constexpr int spirit_summons = 99999007;
static constexpr int materials = 99999008;
static constexpr int miscellaneous_items = 99999009;
static constexpr int gestures = 99999010;
static constexpr int browse_inventory = 99999030;
static constexpr int items = 99999031;
static constexpr int browse_cut_content = 99999032;
static constexpr int goods = 99999033;
static constexpr int unlock = 99999100;
static constexpr int dlc = 99999200;

// Existing messages, for searching for particular talkscript states
static constexpr int about_kale = 28000002;
static constexpr int leave = 20000009;
static constexpr int purchase = 20000010;
static constexpr int sell = 20000011;
}

void setup_messages();
const std::wstring_view get_message(from::msgbnd, int);

extern const std::map<std::string, std::map<int, const std::wstring>> event_text_for_talk_by_lang;

}
