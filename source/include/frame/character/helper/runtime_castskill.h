#pragma once

#include "frame/common/damage.h"
#include <queue>

namespace jx3calc {
namespace frame {

class Character;

class RuntimeCastSkill {
public:
    RuntimeCastSkill(Character *self) :
        self(self) {}
    ~RuntimeCastSkill();

    Character                       *self;
    std::queue<std::tuple<int, int>> skillQueue;
    Damage                           damage;
};

} // namespace frame
} // namespace jx3calc
