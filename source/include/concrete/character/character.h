#ifndef CONCRETE_CHARACTER_ALL_H_
#define CONCRETE_CHARACTER_ALL_H_

#include "frame/character/derived/npc.h"
#include "frame/character/derived/player.h"
#include <memory>
#include <unordered_map>

namespace jx3calc {
namespace concrete {

enum class PlayerType {
    MjFysj,
};

inline const std::unordered_map<std::string, PlayerType> refPlayerType = {
    {"焚影圣诀", PlayerType::MjFysj},
};

std::unique_ptr<frame::Player> createPlayer(PlayerType type, int delayNetwork, int delayKeyboard);

enum class NPCType {
    NPC124,
};

inline const std::unordered_map<std::string, NPCType> refNPCType = {
    {"124级NPC目标", NPCType::NPC124},
};

std::unique_ptr<frame::NPC> createNPC(NPCType type);

} // namespace concrete
} // namespace jx3calc

#endif // CONCRETE_CHARACTER_ALL_H_
