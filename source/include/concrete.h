#pragma once

#include <string>
#include <unordered_map>
namespace jx3calc {
namespace concrete {

enum class Player {
    MjFysj,
};
inline const std::unordered_map<std::string, Player> playerMap{
    {"焚影圣诀", Player::MjFysj},
};

enum class NPC {
    NPC124,
};
inline const std::unordered_map<std::string, NPC> npcMap{
    {"124级NPC目标", NPC::NPC124},
};

} // namespace concrete
} // namespace jx3calc
