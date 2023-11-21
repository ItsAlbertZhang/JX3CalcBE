#ifndef FRAME_CHARACTER_PROPERTY_SKILL_H_
#define FRAME_CHARACTER_PROPERTY_SKILL_H_

#include <queue>
#include <unordered_map>

namespace ns_frame {

class Character;

class CharacterSkill {
public:
    // 已学习技能列表. key 为技能 ID, value 为技能等级.
    std::unordered_map<int, int> skillLearned;

    // 待执行的技能队列.
    class SkillQueueElement {
    public:
        SkillQueueElement(int skillID, int skillLevel) : skillID(skillID), skillLevel(skillLevel) {}
        int skillID;    // 技能 ID
        int skillLevel; // 技能等级
    };
    std::queue<SkillQueueElement> skillQueue;
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_SKILL_H_