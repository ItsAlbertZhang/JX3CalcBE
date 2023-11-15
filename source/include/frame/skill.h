#ifndef MAIN_FRAME_SKILL_H_
#define MAIN_FRAME_SKILL_H_

#include <sol2/sol.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace ns_frame {

class SkillAttribute {
public:
    SkillAttribute(int mode, int type, int param1, int param2)
        : mode(mode), type(type), param1(param1), param2(param2) {}
    int mode;
    int type;
    int param1;
    int param2;
};

class SkillAttributeString {
public:
    SkillAttributeString(int mode, int type, std::string param1, int param2)
        : mode(mode), type(type), param1(param1), param2(param2) {}
    int mode;
    int type;
    std::string param1;
    int param2;
};

/**
 * 技能类
 * 注意: 技能是公共资源, 不属于某个角色. 应当使用技能管理类对其统一进行管理.
 */
class Skill {
public:
    std::unordered_map<std::string, std::string> tab;   // skills.tab 中的数据
    sol::protected_function GetSkillLevelData;          // GetSkillLevelData 函数
    std::vector<SkillAttribute> attributes;             // GetSkillLevelData 函数中添加的属性
    std::vector<SkillAttributeString> attributesString; // GetSkillLevelData 函数中添加的属性
};

/**
 * 技能管理类
 * 这是一个静态类, 用于管理技能数据, 不应当被创建实例.
 */
class SkillManager {
public:
    SkillManager() = delete; // 禁止创建实例

    /**
     * 技能缓存数据
     * 同一 ID, 不同 Level 的技能拥有不同的 Skill 实例.
     */
    static thread_local std::unordered_map<int, std::unordered_map<int, Skill>> data;

    /**
     * @brief 获取技能. 若技能存在, 则命中缓存并返回技能数据; 若技能不存在, 则对其进行初始化并返回技能数据.
     * @param skillID
     * @param skillLevel
     * @return Skill&
     */
    static Skill &get(int skillID, int skillLevel);

private:
    /**
     * @brief 初始化技能. 将指定 ID 与 Level 的技能数据存至缓存.
     * @param skillID
     * @param skillLevel
     */
    static void add(int skillID, int skillLevel);
};

} // namespace ns_frame

#endif // MAIN_FRAME_SKILL_H_