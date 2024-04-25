#pragma once

#include "frame/common/damage.h"
#include "frame/global/skill.h"
#include <queue>
#include <vector>

namespace jx3calc {
namespace frame {

class Character;

class RuntimeCastSkill {
public:
    RuntimeCastSkill(Character *self, int skillID, int skillLevel)
        : self(self), skillID(skillID), skillLevel(skillLevel) {}
    ~RuntimeCastSkill();

    Character                        *self;
    const int                         skillID;
    const int                         skillLevel;
    std::queue<Skill::SkillAttribute> skillQueue;
    std::vector<Damage>               damageList;
};

} // namespace frame
} // namespace jx3calc
