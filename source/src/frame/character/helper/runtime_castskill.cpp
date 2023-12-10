#include "frame/character/helper/runtime_castskill.h"
#include "frame/character/character.h"

using namespace ns_frame;

RuntimeCastSkill::~RuntimeCastSkill() {
    // 执行技能队列
    while (!skillQueue.empty()) {
        auto it = skillQueue.front();
        skillQueue.pop();
        if (it.hasTarget) {
            it.caster->skillCast(it.target, it.skillID, it.skillLevel);
        } else {
            it.caster->skillCast(it.caster->targetCurr, it.skillID, it.skillLevel);
        }
    }
    self->chDamage.damageList.insert(self->chDamage.damageList.end(), damageList.begin(), damageList.end());
}