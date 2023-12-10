#ifndef FRAME_CHARACTER_HELPER_RUNTIME_CASTSKILL_H_
#define FRAME_CHARACTER_HELPER_RUNTIME_CASTSKILL_H_

#include "frame/character/property/damage.h"
#include "frame/event.h"
#include <queue>
#include <vector>

namespace ns_frame {

class Character;

class RuntimeCastSkill {
public:
    RuntimeCastSkill(Character *self, int skillID, int skillLevel)
        : self(self), skillID(skillID), skillLevel(skillLevel) {}
    Character *self;
    const int skillID;
    const int skillLevel;

    // 待执行的技能队列.
    class SkillQueueElement {
    public:
        SkillQueueElement(int skillID, int skillLevel, Character *caster, Character *target)
            : skillID(skillID), skillLevel(skillLevel), caster(caster), target(target) {
            hasTarget = true;
        }
        SkillQueueElement(int skillID, int skillLevel, Character *caster)
            : skillID(skillID), skillLevel(skillLevel), caster(caster) {}
        Character *caster;
        Character *target;
        int skillID;    // 技能 ID
        int skillLevel; // 技能等级
        bool hasTarget = false;
    };
    std::queue<SkillQueueElement> skillQueue;

    std::vector<Damage> damageList;

    ~RuntimeCastSkill();
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_HELPER_RUNTIME_CASTSKILL_H_