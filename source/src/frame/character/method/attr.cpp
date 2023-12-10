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

void Character::attrImportFromJX3BOX(int pzID) {
    static thread_local httplib::Client cli("http://cms.jx3box.com");
    httplib::Result res = cli.Get("/api/cms/app/pz/" + std::to_string(pzID));
    if (res->status != 200) {
        return;
    }
    json j = json::parse(res->body);
    if (!j.contains("code") || j["code"] != 0 || !j.contains("data") || !j["data"].is_object()) {
        return;
    }
    j = j["data"];
    if (!j.contains("data") || !j["data"].is_object()) {
        return;
    }
    j = j["data"];

    chAttr.atVitalityBase = selectPrecise(j, "Vitality"); // 体质
    chAttr.atStrengthBase = selectPrecise(j, "Strength"); // 力道
    chAttr.atAgilityBase = selectPrecise(j, "Agility");   // 身法
    chAttr.atSpiritBase = selectPrecise(j, "Spirit");     // 根骨
    chAttr.atSpunkBase = selectPrecise(j, "Spunk");       // 元气

    chAttr.atPhysicsAttackPowerBase = selectPrecise(j, "PhysicsAttackPowerBase"); // 外功基础攻击
    chAttr.atSolarAttackPowerBase = selectPrecise(j, "SolarAttackPowerBase");     // 阳性内功基础攻击
    chAttr.atLunarAttackPowerBase = selectPrecise(j, "LunarAttackPowerBase");     // 阴性内功基础攻击
    chAttr.atNeutralAttackPowerBase = selectPrecise(j, "NeutralAttackPowerBase"); // 混元内功基础攻击
    chAttr.atPoisonAttackPowerBase = selectPrecise(j, "PoisonAttackPowerBase");   // 毒性内功基础攻击
    chAttr.atTherapyPowerBase = selectPrecise(j, "TherapyPowerBase");             // 治疗量

    chAttr.atPhysicsCriticalStrike = selectPrecise(j, "PhysicsCriticalStrike"); // 外功会心等级
    chAttr.atSolarCriticalStrike = selectPrecise(j, "SolarCriticalStrike");     // 阳性内功会心等级
    chAttr.atLunarCriticalStrike = selectPrecise(j, "LunarCriticalStrike");     // 阴性内功会心等级
    chAttr.atNeutralCriticalStrike = selectPrecise(j, "NeutralCriticalStrike"); // 混元内功会心等级
    chAttr.atPoisonCriticalStrike = selectPrecise(j, "PoisonCriticalStrike");   // 毒性内功会心等级

    chAttr.atPhysicsCriticalDamagePowerBase = selectPrecise(j, "PhysicsCriticalDamagePower"); // 外功会心效果等级
    chAttr.atSolarCriticalDamagePowerBase = selectPrecise(j, "SolarCriticalDamagePower");     // 阳性内功会心效果等级
    chAttr.atLunarCriticalDamagePowerBase = selectPrecise(j, "LunarCriticalDamagePower");     // 阴性内功会心效果等级
    chAttr.atNeutralCriticalDamagePowerBase = selectPrecise(j, "NeutralCriticalDamagePower"); // 混元内功会心效果等级
    chAttr.atPoisonCriticalDamagePowerBase = selectPrecise(j, "PoisonCriticalDamagePower");   // 毒性内功会心效果等级

    chAttr.atPhysicsOvercomeBase = selectPrecise(j, "PhysicsOvercomeBase"); // 外功基础破防等级
    chAttr.atSolarOvercomeBase = selectPrecise(j, "SolarOvercomeBase");     // 阳性内功破防等级
    chAttr.atLunarOvercomeBase = selectPrecise(j, "LunarOvercomeBase");     // 阴性内功破防等级
    chAttr.atNeutralOvercomeBase = selectPrecise(j, "NeutralOvercomeBase"); // 混元内功破防等级
    chAttr.atPoisonOvercomeBase = selectPrecise(j, "PoisonOvercomeBase");   // 毒性内功破防等级

    chAttr.atSurplusValueBase = selectPrecise(j, "SurplusValue"); // 破招值
    chAttr.atStrainBase = selectPrecise(j, "Strain");             // 无双等级

    // chAttr.atHasteBase = selectPrecise(j, "Haste"); // 基础加速等级
    chAttr.atHasteBase = 95; // TODO: 加速酒还没做 (还有网络延迟)

    chAttr.atPhysicsShieldBase = selectPrecise(j, "PhysicsShieldBase"); // 外功基础防御等级

    chAttr.atSolarMagicShieldBase = selectPrecise(j, "SolarShieldBase");     // 阳性内功防御等级
    chAttr.atLunarMagicShieldBase = selectPrecise(j, "LunarShieldBase");     // 阴性内功防御等级
    chAttr.atNeutralMagicShieldBase = selectPrecise(j, "NeutralShieldBase"); // 混元内功防御等级
    chAttr.atPoisonMagicShieldBase = selectPrecise(j, "PoisonShieldBase");   // 毒性内功防御等级

    chAttr.atDodge = selectPrecise(j, "Dodge"); // 闪避等级

    chAttr.atParryBase = selectPrecise(j, "Parry"); // 招架等级

    chAttr.atParryValueBase = selectPrecise(j, "ParryValue"); // 拆招值

    chAttr.atToughnessBase = selectPrecise(j, "Toughness"); // 御劲等级

    chAttr.atDecriticalDamagePowerBase = selectPrecise(j, "DecriticalDamage"); // 化劲等级

    chAttr.atMeleeWeaponDamageBase = selectPrecise(j, "MeleeWeaponDamage");     // 武器伤害
    chAttr.atMeleeWeaponDamageRand = selectPrecise(j, "MeleeWeaponDamageRand"); // 武器伤害浮动
}

void Character::attrImportFromBackup(const CharacterAttr &attr) {
    chAttr = attr;
}

CharacterAttr Character::attrExport() {
    return chAttr;
}