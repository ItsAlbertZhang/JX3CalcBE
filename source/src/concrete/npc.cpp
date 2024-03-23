#include "concrete/npc.h"

using namespace jx3calc;
using namespace concrete;

namespace {
class NPC124 : public frame::NPC {
public:
    NPC124() {
        nLevel                     = 124;
        chAttr.atLevel             = 124;
        chAttr.atPhysicsShieldBase = 27550;
        chAttr.atMagicShield       = 27550;
        fMaxLife64                 = 1e+10;
        fCurrentLife64             = 1e+10;
    }
};
} // namespace

auto jx3calc::concrete::create(npc::Type type) -> std::unique_ptr<frame::NPC> {
    switch (type) {
    case npc::Type::NPC124:
        return std::make_unique<NPC124>();
    }
}
