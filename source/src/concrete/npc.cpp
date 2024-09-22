#include "concrete.h"
#include "modules/config.h"

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

template <>
auto concrete::createNPC<concrete::NPCType::NPCatLevelAdd4, modules::config::ClientType::jx3_hd>() -> std::unique_ptr<frame::NPC> {
    return std::make_unique<NPC124>();
}
template <>
auto concrete::createNPC<concrete::NPCType::NPCatLevelAdd4, modules::config::ClientType::jx3_exp>() -> std::unique_ptr<frame::NPC> {
    return std::make_unique<NPC134>();
}

auto concrete::createNPC(NPCType type) -> std::unique_ptr<frame::NPC> {
    const std::unordered_map<NPCType, std::unordered_map<modules::config::ClientType, std::function<std::unique_ptr<frame::NPC>()>>> funcmap {
        {NPCType::NPCatLevelAdd4,
         {
             {modules::config::ClientType::jx3_hd, []() { return createNPC<NPCType::NPCatLevelAdd4, modules::config::ClientType::jx3_hd>(); }},
             {modules::config::ClientType::jx3_exp, []() { return createNPC<NPCType::NPCatLevelAdd4, modules::config::ClientType::jx3_exp>(); }},
         }},
    };
    return funcmap.at(type).at(modules::config::clientType)();
}
