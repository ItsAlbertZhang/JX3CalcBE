#include "frame/global/uibuff.h"
#include "frame/global/buff.h"
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

const UIBuff &UIBuffManager::get(int buffID, int buffLevel) {
    // 若 UIBuff ID 和 Level 任一不存在, 则添加
    if (data.find(std::make_tuple(buffID, buffLevel)) == data.end()) {
        add(buffID, buffLevel);
    }
    return data[std::make_tuple(buffID, buffLevel)];
}

void UIBuffManager::add(int buffID, int buffLevel) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断
    // UIBuff 是否存在.
    if (data.find(std::make_tuple(buffID, buffLevel)) != data.end()) {
        // 若 UIBuff ID 存在, 且 UIBuff 等级存在, 则直接返回
        return; // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }

    // 初始化 buff
    UIBuff buff;
    buff.ID = buffID;
    buff.Level = buffLevel;

    // 获取 tab
    gdi::TabSelectType arg;
    arg.emplace_back();
    /* Precise select first */
    arg[0]["BuffID"] = std::to_string(buffID);
    arg[0]["Level"] = std::to_string(buffLevel);
    gdi::Interface::tabSelect(gdi::Tab::ui_buff, arg);
    if (arg.size() == 0) {
        /* Fuzzy select */
        arg.emplace_back();
        arg[0]["BuffID"] = std::to_string(buffID);
        arg[0]["Level"] = "0";
        gdi::Interface::tabSelect(gdi::Tab::ui_buff, arg);
    }
    buff.tab = std::move(arg[0]);

    buff.Name = buff.tab["Name"];

    // 将 UIBuff 存入缓存
    data[std::make_tuple(buffID, buffLevel)] = std::move(buff);
}

// std::string UIBuffManager::representDamage(Damage &damage) {
//     std::stringstream ss;
//     /* FIXME: Replace this after proper implementation of damage source */
//     auto skill = ns_frame::SkillManager::get(damage.skillID, damage.skillLevel);
//     ss << "[" << std::fixed << std::setprecision(2) << damage.tick / 1024.0
//        << "s][雨轩]的[" << skill.SkillName
//        << (damage.isCritical ? "(会心)" : "") << "]对[雨轩的木桩]造成了"
//        << DamageToString(damage.damage, damage.damageType) << "伤害。"
//        << std::endl;
//     return ss.str();
// }