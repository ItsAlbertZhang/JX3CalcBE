#include "concrete.h"

using namespace jx3calc;

namespace {

class NPC134 : public frame::NPC {
public:
    NPC134() {
        chAttr.atLevel             = 134;
        chAttr.atPhysicsShieldBase = 83679;
        chAttr.atMagicShield       = 83679;
        fMaxLife64                 = 1e+10;
        fCurrentLife64             = 1e+10;
    }
};

} // namespace

auto concrete::createNPC(NPCType type) -> std::unique_ptr<frame::NPC> {
    return std::make_unique<NPC134>();
}
