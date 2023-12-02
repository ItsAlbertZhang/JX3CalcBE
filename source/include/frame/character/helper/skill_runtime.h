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

    // 待执行的技能队列.
    class SkillQueueElement {
    public:
        SkillQueueElement(int skillID, int skillLevel, Character *caster, Character *target)
            : skillID(skillID), skillLevel(skillLevel), caster(caster), target(target) {}
        Character *caster;
        Character *target;
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
        // 从技能运行时栈中弹出
        runtimeStack.pop();
    }
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_HELPER_SKILL_RUNTIME_H_