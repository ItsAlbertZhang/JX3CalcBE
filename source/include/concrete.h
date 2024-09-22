#pragma once

#include "frame/character/effect.h"
#include "frame/character/npc.h"
#include "frame/character/player.h"
#include "modules/config.h"
#include <string>
#include <unordered_map>

namespace jx3calc {
namespace concrete {

enum class PlayerType {
    MjFysj,
};
inline const std::unordered_map<std::string, PlayerType> playerMap {
    {"焚影圣诀", PlayerType::MjFysj},
};

enum class NPCType {
    NPCatLevelAdd4,
};
inline const std::unordered_map<std::string, NPCType> npcMap {
    {"高4级NPC目标", NPCType::NPCatLevelAdd4},
};

// 内部实现使用模板函数, 方便在不同 .cpp 下进行实例化.
template <PlayerType type, modules::config::ClientType>
extern auto createPlayer() -> std::unique_ptr<frame::Player>;
template <NPCType type, modules::config::ClientType>
extern auto createNPC() -> std::unique_ptr<frame::NPC>;
template <modules::config::ClientType>
extern auto createEffect(const std::string &type, const std::string &jsonstr) -> std::shared_ptr<frame::Effect>;

// 外部接口
std::unique_ptr<frame::Player> createPlayer(PlayerType type);
std::unique_ptr<frame::NPC>    createNPC(NPCType type);
std::shared_ptr<frame::Effect> createEffect(const std::string &type, const std::string &jsonstr);

void dummyFunc(); // fix unused includes warning

} // namespace concrete
} // namespace jx3calc
