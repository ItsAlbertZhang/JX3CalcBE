#include "frame/character/character.h"
#include <httplib.h>
#include <nlohmann/json.hpp>

using namespace ns_frame;
using json = nlohmann::json;

static inline int selectPrecise(const json &j, const std::string &key) {
    int ret = 0;
    if (j.contains(key) && j[key].is_number_integer()) {
        ret = j[key];
    }
    return ret;
}

// static inline int selectFuzzy(const json &j, const std::string &subkey, const std::string &notkey) {
//     for (json::iterator it = j.begin(); it != j.end(); it++) {
//         if (it.key().find(subkey) != std::string::npos && it.key().find(notkey) == std::string::npos && it.value().is_number_integer()) {
//             return it.value();
//         }
//     }
//     return 0;
// }

bool Character::attrExportFromJX3BOX(int pzID, ChAttr &attr) {
    static thread_local httplib::Client cli("http://cms.jx3box.com");
    httplib::Result                     res = cli.Get("/api/cms/app/pz/" + std::to_string(pzID));
    if (res->status != 200) {
        return false;
    }
    json j = json::parse(res->body);
    if (!j.contains("code") || j["code"] != 0 || !j.contains("data") || !j["data"].is_object()) {
        return false;
    }
    j = j["data"];
    if (!j.contains("data") || !j["data"].is_object()) {
        return false;
    }
    j = j["data"];

    attr.atVitalityBase = selectPrecise(j, "Vitality"); // 体质
    attr.atStrengthBase = selectPrecise(j, "Strength"); // 力道
    attr.atAgilityBase  = selectPrecise(j, "Agility");  // 身法
    attr.atSpiritBase   = selectPrecise(j, "Spirit");   // 根骨
    attr.atSpunkBase    = selectPrecise(j, "Spunk");    // 元气

    attr.atPhysicsAttackPowerBase = selectPrecise(j, "PhysicsAttackPowerBase"); // 外功基础攻击
    attr.atSolarAttackPowerBase   = selectPrecise(j, "SolarAttackPowerBase");   // 阳性内功基础攻击
    attr.atLunarAttackPowerBase   = selectPrecise(j, "LunarAttackPowerBase");   // 阴性内功基础攻击
    attr.atNeutralAttackPowerBase = selectPrecise(j, "NeutralAttackPowerBase"); // 混元内功基础攻击
    attr.atPoisonAttackPowerBase  = selectPrecise(j, "PoisonAttackPowerBase");  // 毒性内功基础攻击
    attr.atTherapyPowerBase       = selectPrecise(j, "TherapyPowerBase");       // 治疗量

    attr.atPhysicsCriticalStrike = selectPrecise(j, "PhysicsCriticalStrike"); // 外功会心等级
    attr.atSolarCriticalStrike   = selectPrecise(j, "SolarCriticalStrike");   // 阳性内功会心等级
    attr.atLunarCriticalStrike   = selectPrecise(j, "LunarCriticalStrike");   // 阴性内功会心等级
    attr.atNeutralCriticalStrike = selectPrecise(j, "NeutralCriticalStrike"); // 混元内功会心等级
    attr.atPoisonCriticalStrike  = selectPrecise(j, "PoisonCriticalStrike");  // 毒性内功会心等级

    attr.atPhysicsCriticalDamagePowerBase = selectPrecise(j, "PhysicsCriticalDamagePower"); // 外功会心效果等级
    attr.atSolarCriticalDamagePowerBase   = selectPrecise(j, "SolarCriticalDamagePower");   // 阳性内功会心效果等级
    attr.atLunarCriticalDamagePowerBase   = selectPrecise(j, "LunarCriticalDamagePower");   // 阴性内功会心效果等级
    attr.atNeutralCriticalDamagePowerBase = selectPrecise(j, "NeutralCriticalDamagePower"); // 混元内功会心效果等级
    attr.atPoisonCriticalDamagePowerBase  = selectPrecise(j, "PoisonCriticalDamagePower");  // 毒性内功会心效果等级

    attr.atPhysicsOvercomeBase = selectPrecise(j, "PhysicsOvercomeBase"); // 外功基础破防等级
    attr.atSolarOvercomeBase   = selectPrecise(j, "SolarOvercomeBase");   // 阳性内功破防等级
    attr.atLunarOvercomeBase   = selectPrecise(j, "LunarOvercomeBase");   // 阴性内功破防等级
    attr.atNeutralOvercomeBase = selectPrecise(j, "NeutralOvercomeBase"); // 混元内功破防等级
    attr.atPoisonOvercomeBase  = selectPrecise(j, "PoisonOvercomeBase");  // 毒性内功破防等级

    attr.atSurplusValueBase = selectPrecise(j, "SurplusValue"); // 破招值
    attr.atStrainBase       = selectPrecise(j, "Strain");       // 无双等级

    // chAttr.atHasteBase = selectPrecise(j, "Haste"); // 基础加速等级
    attr.atHasteBase = 95; // TODO: 加速酒还没做 (还有网络延迟)

    attr.atPhysicsShieldBase = selectPrecise(j, "PhysicsShieldBase"); // 外功基础防御等级

    attr.atSolarMagicShieldBase   = selectPrecise(j, "SolarShieldBase");   // 阳性内功防御等级
    attr.atLunarMagicShieldBase   = selectPrecise(j, "LunarShieldBase");   // 阴性内功防御等级
    attr.atNeutralMagicShieldBase = selectPrecise(j, "NeutralShieldBase"); // 混元内功防御等级
    attr.atPoisonMagicShieldBase  = selectPrecise(j, "PoisonShieldBase");  // 毒性内功防御等级

    attr.atDodge = selectPrecise(j, "Dodge"); // 闪避等级

    attr.atParryBase = selectPrecise(j, "Parry"); // 招架等级

    attr.atParryValueBase = selectPrecise(j, "ParryValue"); // 拆招值

    attr.atToughnessBase = selectPrecise(j, "Toughness"); // 御劲等级

    attr.atDecriticalDamagePowerBase = selectPrecise(j, "DecriticalDamage"); // 化劲等级

    attr.atMeleeWeaponDamageBase = selectPrecise(j, "MeleeWeaponDamage");     // 武器伤害
    attr.atMeleeWeaponDamageRand = selectPrecise(j, "MeleeWeaponDamageRand"); // 武器伤害浮动

    return true;
}

bool Character::attrImportFromJX3BOX(int pzID) {
    return attrExportFromJX3BOX(pzID, chAttr);
}

bool Character::attrImportFromBackup(const ChAttr &attr) {
    chAttr = attr;
    return true;
}
