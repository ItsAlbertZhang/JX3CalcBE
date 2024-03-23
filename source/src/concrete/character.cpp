#include "concrete/character/character.h"
#include "concrete/character/npc/npc124.h"
#include "concrete/character/player/mj_fysj.h"
#include "frame/character/derived/player.h"
#include <memory>

using namespace jx3calc;
using namespace concrete;

std::unique_ptr<frame::Player> concrete::createPlayer(PlayerType type, int delayNetwork, int delayKeyboard) {
    switch (type) {
    case PlayerType::MjFysj:
        return std::make_unique<MjFysj>(delayNetwork, delayKeyboard);
    }
}

std::unique_ptr<frame::NPC> concrete::createNPC(NPCType type) {
    switch (type) {
    case NPCType::NPC124:
        return std::make_unique<NPC124>();
    }
}
