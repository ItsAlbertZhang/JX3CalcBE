#ifndef FRAME_CHARACTER_PROPERTY_SKILL_H_
#define FRAME_CHARACTER_PROPERTY_SKILL_H_

#include <queue>
#include <unordered_map>

namespace ns_frame {

class CharacterSkill {
public:
    // 已学习技能列表. key 为技能 ID, value 为技能等级.
    std::unordered_map<int, int> skillLearned;

    class SkillActived {
    public:
        SkillActived(int level, void *attribute) : level(level), attribute(attribute) {}
        int level;       // 技能等级
        void *attribute; // 魔法属性对象
    };
    /**
     * @brief 激活的技能列表
     * @note key 为 SkillID
     */
    std::unordered_map<int, SkillActived> skillActived;

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