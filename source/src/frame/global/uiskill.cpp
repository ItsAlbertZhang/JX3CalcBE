#include "frame/global/uiskill.h"
#include "frame/global/skill.h"
#include "gdi.h"
#include "program/log.h"

using namespace ns_frame;

// inline std::string DamageToString(int damage, DamageType damageType) {
//     switch (damageType) {
//     case DamageType::Physics:
//         return "\033[37m" + std::to_string(damage) + "点外功\033[0m"; // White
//     case DamageType::Solar:
//         return "\033[31m" + std::to_string(damage) + "点阳性内功\033[0m"; // Red
//     case DamageType::Lunar:
//         return "\033[94m" + std::to_string(damage) + "点阴性内功\033[0m"; // Blue
//     case DamageType::Neutral:
//         return "\033[33m" + std::to_string(damage) +
//                "点混元性内功\033[0m"; // Yellow
//     case DamageType::Poison:
//         return "\033[35m" + std::to_string(damage) + "点毒性内功\033[0m"; // Magenta
//     default:
//         return "Unknown";
//     }
// }

const UISkill &UISkillManager::get(int skillID, int skillLevel) {
    // 若 UISkill ID 和 Level 任一不存在, 则添加
    if (data.find(std::make_tuple(skillID, skillLevel)) == data.end()) {
        add(skillID, skillLevel);
    }
    return data[std::make_tuple(skillID, skillLevel)];
}

void UISkillManager::add(int skillID, int skillLevel) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断
    // UISkill 是否存在.
    if (data.find(std::make_tuple(skillID, skillLevel)) != data.end()) {
        // 若 UISkill ID 存在, 且 UISkill 等级存在, 则直接返回
        return; // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }

    // 初始化 skill
    UISkill skill;
    skill.ID    = skillID;
    skill.Level = skillLevel;
    // 获取 tab
    gdi::select_t arg;
    arg.emplace_back();
    /* Precise select first */
    arg[0]["SkillID"] = std::to_string(skillID);
    arg[0]["Level"]   = std::to_string(skillLevel);
    arg.emplace_back();
    arg[1]["SkillID"] = std::to_string(skillID);
    arg[1]["Level"]   = "0";
    gdi::tabSelect(gdi::Tab::ui_skill, arg);
    // if (arg.size() == 0) {
    //     /* FIXME: Replace this after proper implementation of damage source */
    //     skill.tab["Name"] = ns_frame::SkillManager::get(skillID, skillLevel).SkillName;
    //     skill.Name = skill.tab["Name"];
    //     data[std::make_tuple(skillID, skillLevel)] = std::move(skill);
    //     return;
    // }
    skill.tab = std::move(arg[0]);

    skill.Name = skill.tab["Name"];

    data[std::make_tuple(skillID, skillLevel)] = std::move(skill);
}

// std::string UISkillManager::representDamage(Damage &damage) {
//     std::stringstream ss;
//     auto skill = ns_frame::SkillManager::get(damage.skillID, damage.skillLevel);
//     ss << "[" << std::fixed << std::setprecision(2) << damage.tick / 1024.0
//        << "s][雨轩]的[" << get(damage.skillID, damage.skillLevel).Name
//        << (damage.isCritical ? "(会心)" : "") << "]对[雨轩的木桩]造成了"
//        << DamageToString(damage.damage, damage.damageType) << "伤害。"
//        << std::endl;
//     return ss.str();
// }
