#include "concrete/character.h"
#include "concrete/character/npc/npc124.h"
#include "concrete/character/player/mj_fysj.h"
#include "frame/character/derived/player.h"
#include <memory>

using namespace ns_concrete;

std::unique_ptr<ns_frame::Player> ns_concrete::createPlayer(PlayerType type, int delayNetwork, int delayKeyboard) {
    switch (type) {
    case PlayerType::MjFysj:
        return std::make_unique<MjFysj>(delayNetwork, delayKeyboard);
    default:
        return nullptr;
    }
}

std::unique_ptr<ns_frame::NPC> ns_concrete::createNPC(NPCType type) {
    switch (type) {
    case NPCType::NPC124:
        return std::make_unique<NPC124>();
    default:
        return nullptr;
    }
}
