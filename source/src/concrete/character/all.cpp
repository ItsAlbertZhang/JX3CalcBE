#include "concrete/character/all.h"
#include "concrete/character/npc/npc124.h"
#include "concrete/character/player/mj_fysj.h"
#include "frame/character/derived/player.h"
#include <memory>

using namespace ns_concrete;

std::unique_ptr<ns_frame::Player> PlayerManager::create(PlayerType type, int delayNetwork, int delayKeyboard) {
    switch (type) {
    case PlayerType::MjFysj:
        return std::make_unique<MjFysj>(delayNetwork, delayKeyboard);
    default:
        throw std::runtime_error{"PlayerManager::create: invalid type"};
    }
}

std::unique_ptr<ns_frame::NPC> NPCManager::create(NPCType type) {
    switch (type) {
    case NPCType::NPC124:
        return std::make_unique<NPC124>();
    default:
        throw std::runtime_error{"NPCManager::create: invalid type"};
    }
}
