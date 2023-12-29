#ifndef CONCRETE_CHARACTER_PLAYER_ALL_H_
#define CONCRETE_CHARACTER_PLAYER_ALL_H_

#include "frame/character/derived/npc.h"
#include "frame/character/derived/player.h"
#include <memory>

namespace ns_concrete {

enum class PlayerType {
    MjFysj,
    COUNT,
};

inline const char *PlayerTypeName[] = {
    "焚影圣诀",
};

class PlayerManager {
public:
    PlayerManager() = delete;
    static std::unique_ptr<ns_frame::Player> create(PlayerType type, int delayNetwork, int delayKeybord);
};

enum class NPCType {
    NPC124,
    COUNT,
};

inline const char *NPCTypeName[] = {
    "124级NPC目标",
};

class NPCManager {
public:
    NPCManager() = delete;
    static std::unique_ptr<ns_frame::NPC> create(NPCType type);
};

} // namespace ns_concrete

#endif // CONCRETE_CHARACTER_PLAYER_ALL_H_
