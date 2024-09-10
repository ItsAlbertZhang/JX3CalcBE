#pragma once

#include <string>
#include <unordered_map>
namespace jx3calc {
namespace concrete {

enum class Player {
    MjFysj,
};
inline const std::unordered_map<std::string, Player> playerMap {
    {"焚影圣诀", Player::MjFysj},
};

enum class NPC {
    NPCatLevelAdd4,
};
inline const std::unordered_map<std::string, NPC> npcMap {
    {"高4级NPC目标", NPC::NPCatLevelAdd4},
};

} // namespace concrete
} // namespace jx3calc
