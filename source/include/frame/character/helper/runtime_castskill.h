#ifndef FRAME_CHARACTER_HELPER_RUNTIME_CASTSKILL_H_
#define FRAME_CHARACTER_HELPER_RUNTIME_CASTSKILL_H_

#include "frame/character/property/damage.h"
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

    // 待执行的技能队列.
    class SkillQueueElement {
    public:
        SkillQueueElement(int skillID, int skillLevel, Character *caster, Character *target)
            : skillID(skillID), skillLevel(skillLevel), caster(caster), target(target), hasTarget(true) {}
        SkillQueueElement(int skillID, int skillLevel, Character *caster)
            : skillID(skillID), skillLevel(skillLevel), caster(caster) {}

        int        skillID;    // 技能 ID
        int        skillLevel; // 技能等级
        Character *caster;
        Character *target;

        bool hasTarget = false;
    };

    Character                    *self;
    const int                     skillID;
    const int                     skillLevel;
    std::queue<SkillQueueElement> skillQueue;
    std::vector<Damage>           damageList;
};

} // namespace frame
} // namespace jx3calc

#endif // FRAME_CHARACTER_HELPER_RUNTIME_CASTSKILL_H_
