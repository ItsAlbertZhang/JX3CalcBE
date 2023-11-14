#pragma once

#include "concrete.h"
#include "frame/character/character.h"
#include <memory>

namespace jx3calc {

namespace frame {
class NPC : public Character {};
} // namespace frame

namespace concrete {
auto createNPC(NPC type) -> std::unique_ptr<frame::NPC>;
}

} // namespace jx3calc
