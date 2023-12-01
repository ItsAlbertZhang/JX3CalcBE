#ifndef FRAME_CHARACTER_HELPER_SKILL_RUNTIME_H_
#define FRAME_CHARACTER_HELPER_SKILL_RUNTIME_H_

#include "frame/character/character.h"
#include "frame/event.h"
#include <queue>
#include <stack>

namespace ns_frame {

class SkillRuntime {
public:
    SkillRuntime(Character *self, int skillID, int skillLevel)
        : self(self), skillID(skillID), skillLevel(skillLevel) {
        runtimeStack.push(this); // 将自己压入技能运行时栈
    }
    Character *self;
    const int skillID;
    const int skillLevel;
    bool isCritical = false;
    int dmgPhysics = 0;
    int dmgSolar = 0;
    int dmgLunar = 0;
    int dmgNeutral = 0;
    int dmgPoison = 0;

    // 待执行的技能队列.
    class SkillQueueElement {
    public:
        SkillQueueElement(int skillID, int skillLevel) : skillID(skillID), skillLevel(skillLevel) {}
        int skillID;    // 技能 ID
        int skillLevel; // 技能等级
    };
    std::queue<SkillQueueElement> skillQueue;
    static thread_local inline std::stack<SkillRuntime *> runtimeStack;

    ~SkillRuntime() {
        // 执行技能队列
        while (!skillQueue.empty()) {
            auto it = skillQueue.front();
            skillQueue.pop();
            self->CastSkill(it.skillID, it.skillLevel);
        }
        // 将伤害添加至 self.chDamage
        if (dmgPhysics > 0) {
            self->chDamage.damageList.emplace_back(
                Event::now(),
                skillID, skillLevel,
                isCritical,
                dmgPhysics,
                DamageType::Physics);
            self->isOutOfFight = false;
        }
        if (dmgSolar > 0) {
            self->chDamage.damageList.emplace_back(
                Event::now(),
                skillID, skillLevel,
                isCritical,
                dmgSolar,
                DamageType::Solar);
            self->isOutOfFight = false;
        }
        if (dmgLunar > 0) {
            self->chDamage.damageList.emplace_back(
                Event::now(),
                skillID, skillLevel,
                isCritical,
                dmgLunar,
                DamageType::Lunar);
            self->isOutOfFight = false;
        }
        if (dmgNeutral > 0) {
            self->chDamage.damageList.emplace_back(
                Event::now(),
                skillID, skillLevel,
                isCritical,
                dmgNeutral,
                DamageType::Neutral);
            self->isOutOfFight = false;
        }
        if (dmgPoison > 0) {
            self->chDamage.damageList.emplace_back(
                Event::now(),
                skillID, skillLevel,
                isCritical,
                dmgPoison,
                DamageType::Poison);
            self->isOutOfFight = false;
        }
        // 从技能运行时栈中弹出
        runtimeStack.pop();
    }
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_HELPER_SKILL_RUNTIME_H_