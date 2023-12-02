#ifndef FRAME_CHARACTER_HELPER_SKILL_RUNTIME_H_
#define FRAME_CHARACTER_HELPER_SKILL_RUNTIME_H_

#include "frame/character/character.h"
#include "frame/event.h"
#include <queue>
#include <vector>

namespace ns_frame {

class SkillRuntime {
public:
    SkillRuntime(Character *self, int skillID, int skillLevel)
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

    ~SkillRuntime() {
        // 执行技能队列
        while (!skillQueue.empty()) {
            auto it = skillQueue.front();
            skillQueue.pop();
            if (it.hasTarget) {
                it.caster->CastSkill(it.target, it.skillID, it.skillLevel);
            } else {
                it.caster->CastSkill(it.caster->targetCurr, it.skillID, it.skillLevel);
            }
        }
        self->chDamage.damageList.insert(self->chDamage.damageList.end(), damageList.begin(), damageList.end());
    }
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_HELPER_SKILL_RUNTIME_H_