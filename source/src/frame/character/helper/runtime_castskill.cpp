#include "frame/character/helper/runtime_castskill.h"
#include "frame/character/character.h"

using namespace jx3calc;
using namespace frame;

RuntimeCastSkill::~RuntimeCastSkill() {
    while (!skillQueue.empty()) {
        auto it = std::move(skillQueue.front());
        skillQueue.pop();
        self->skillCast(std::get<0>(it), std::get<1>(it));
    }
    if (damage.id != 0) {
        self->chDamage.emplace_back(std::move(damage));
    }
}
