#ifndef FRAME_CHARACTER_PROPERTY_SKILL_H_
#define FRAME_CHARACTER_PROPERTY_SKILL_H_

#include <unordered_map>

namespace ns_frame {

class ChSkill {
public:
    class SkillActived {
    public:
        SkillActived(int level, void *attribute)
            : level(level), attribute(attribute) {}

        int   level;     // 技能等级
        void *attribute; // 魔法属性对象
    };

    std::unordered_map<int, int>          skillLearned; // 已学习技能列表, 技能 ID - 技能等级.
    std::unordered_map<int, SkillActived> skillActived; // 激活的技能列表, 技能 ID - SkillActived.
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_SKILL_H_
