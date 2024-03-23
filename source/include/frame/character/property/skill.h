#ifndef FRAME_CHARACTER_PROPERTY_SKILL_H_
#define FRAME_CHARACTER_PROPERTY_SKILL_H_

#include <string>
#include <unordered_map>

namespace jx3calc {
namespace frame {

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
    std::unordered_map<std::string, int>  skillRef;     // 技能反查表, 技能名 - 技能 ID.
};

} // namespace frame
} // namespace jx3calc

#endif // FRAME_CHARACTER_PROPERTY_SKILL_H_
