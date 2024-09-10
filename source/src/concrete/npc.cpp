#include "frame/character/derived/npc.h"
#include "concrete.h"
#include <stdexcept>

using namespace jx3calc;

namespace {
class NPC124 : public frame::NPC {
public:
    NPC124() {
        chAttr.atLevel             = 124;
        chAttr.atPhysicsShieldBase = 27550;
        chAttr.atMagicShield       = 27550;
        fMaxLife64                 = 1e+10;
        fCurrentLife64             = 1e+10;
    }
};
} // namespace

auto concrete::createNPC(NPC type) -> std::unique_ptr<frame::NPC> {
    switch (type) {
    case NPC::NPC124:
        return std::make_unique<NPC124>();
    default:
        throw std::range_error("Invalid npcType");
    }
}
