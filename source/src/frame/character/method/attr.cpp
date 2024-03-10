#include "frame/character/character.h"
#include <nlohmann/json.hpp>

#pragma warning(push) // MSVC
#pragma warning(disable : 4100)
#include <httplib.h> // (:4795): warning C4100: “content_length”: 未引用的形参
#pragma warning(pop) // MSVC

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

static void attrImport(Character *self, const json &j) {
    self->chAttr.atVitalityBase = selectPrecise(j, "Vitality"); // 体质
    self->chAttr.atStrengthBase = selectPrecise(j, "Strength"); // 力道
    self->chAttr.atAgilityBase  = selectPrecise(j, "Agility");  // 身法
    self->chAttr.atSpiritBase   = selectPrecise(j, "Spirit");   // 根骨
    self->chAttr.atSpunkBase    = selectPrecise(j, "Spunk");    // 元气

    self->chAttr.atPhysicsAttackPowerBase = selectPrecise(j, "PhysicsAttackPowerBase"); // 外功基础攻击
    self->chAttr.atSolarAttackPowerBase   = selectPrecise(j, "SolarAttackPowerBase");   // 阳性内功基础攻击
    self->chAttr.atLunarAttackPowerBase   = selectPrecise(j, "LunarAttackPowerBase");   // 阴性内功基础攻击
    self->chAttr.atNeutralAttackPowerBase = selectPrecise(j, "NeutralAttackPowerBase"); // 混元内功基础攻击
    self->chAttr.atPoisonAttackPowerBase  = selectPrecise(j, "PoisonAttackPowerBase");  // 毒性内功基础攻击
    self->chAttr.atTherapyPowerBase       = selectPrecise(j, "TherapyPowerBase");       // 治疗量

    // 额外处理心法转换的会心. 攻击和破防不需要额外处理, 因为这两者均区分基础/额外.
    self->chAttr.atPhysicsCriticalStrike = 0; // 外功会心等级
    self->chAttr.atPhysicsCriticalStrike = selectPrecise(j, "PhysicsCriticalStrike") - self->chAttr.getPhysicsCriticalStrikeValue();
    self->chAttr.atSolarCriticalStrike   = 0; // 阳性内功会心等级
    self->chAttr.atSolarCriticalStrike   = selectPrecise(j, "SolarCriticalStrike") - self->chAttr.getSolarCriticalStrikeValue();
    self->chAttr.atLunarCriticalStrike   = 0; // 阴性内功会心等级
    self->chAttr.atLunarCriticalStrike   = selectPrecise(j, "LunarCriticalStrike") - self->chAttr.getLunarCriticalStrikeValue();
    self->chAttr.atNeutralCriticalStrike = 0; // 混元内功会心等级
    self->chAttr.atNeutralCriticalStrike = selectPrecise(j, "NeutralCriticalStrike") - self->chAttr.getNeutralCriticalStrikeValue();
    self->chAttr.atPoisonCriticalStrike  = 0; // 毒性内功会心等级
    self->chAttr.atPoisonCriticalStrike  = selectPrecise(j, "PoisonCriticalStrike") - self->chAttr.getPoisonCriticalStrikeValue();

    self->chAttr.atPhysicsCriticalDamagePowerBase = selectPrecise(j, "PhysicsCriticalDamagePower"); // 外功会心效果等级
    self->chAttr.atSolarCriticalDamagePowerBase   = selectPrecise(j, "SolarCriticalDamagePower");   // 阳性内功会心效果等级
    self->chAttr.atLunarCriticalDamagePowerBase   = selectPrecise(j, "LunarCriticalDamagePower");   // 阴性内功会心效果等级
    self->chAttr.atNeutralCriticalDamagePowerBase = selectPrecise(j, "NeutralCriticalDamagePower"); // 混元内功会心效果等级
    self->chAttr.atPoisonCriticalDamagePowerBase  = selectPrecise(j, "PoisonCriticalDamagePower");  // 毒性内功会心效果等级

    self->chAttr.atPhysicsOvercomeBase = selectPrecise(j, "PhysicsOvercomeBase"); // 外功基础破防等级
    self->chAttr.atSolarOvercomeBase   = selectPrecise(j, "SolarOvercomeBase");   // 阳性内功破防等级
    self->chAttr.atLunarOvercomeBase   = selectPrecise(j, "LunarOvercomeBase");   // 阴性内功破防等级
    self->chAttr.atNeutralOvercomeBase = selectPrecise(j, "NeutralOvercomeBase"); // 混元内功破防等级
    self->chAttr.atPoisonOvercomeBase  = selectPrecise(j, "PoisonOvercomeBase");  // 毒性内功破防等级

    self->chAttr.atSurplusValueBase = selectPrecise(j, "SurplusValue"); // 破招值
    self->chAttr.atStrainBase       = selectPrecise(j, "Strain");       // 无双等级

    self->chAttr.atHasteBase = selectPrecise(j, "Haste"); // 基础加速等级

    self->chAttr.atPhysicsShieldBase = selectPrecise(j, "PhysicsShieldBase"); // 外功基础防御等级

    self->chAttr.atSolarMagicShieldBase   = selectPrecise(j, "SolarShieldBase");   // 阳性内功防御等级
    self->chAttr.atLunarMagicShieldBase   = selectPrecise(j, "LunarShieldBase");   // 阴性内功防御等级
    self->chAttr.atNeutralMagicShieldBase = selectPrecise(j, "NeutralShieldBase"); // 混元内功防御等级
    self->chAttr.atPoisonMagicShieldBase  = selectPrecise(j, "PoisonShieldBase");  // 毒性内功防御等级

    self->chAttr.atDodge = selectPrecise(j, "Dodge"); // 闪避等级

    self->chAttr.atParryBase = selectPrecise(j, "Parry"); // 招架等级

    self->chAttr.atParryValueBase = selectPrecise(j, "ParryValue"); // 拆招值

    self->chAttr.atToughnessBase = selectPrecise(j, "Toughness"); // 御劲等级

    self->chAttr.atDecriticalDamagePowerBase = selectPrecise(j, "DecriticalDamage"); // 化劲等级

    self->chAttr.atMeleeWeaponDamageBase = selectPrecise(j, "MeleeWeaponDamage");     // 武器伤害
    self->chAttr.atMeleeWeaponDamageRand = selectPrecise(j, "MeleeWeaponDamageRand"); // 武器伤害浮动
}

bool Character::attrImportFromData(std::string dataJsonStr) {
    try {
        json j = json::parse(dataJsonStr);
        if (!j.is_object()) {
            return false;
        }
        attrImport(this, j);
        return true;
    } catch (...) {
        return false;
    }
}

bool Character::attrImportFromJX3BOX(std::string pzID) {
    static thread_local httplib::Client cli("http://cms.jx3box.com");
    httplib::Result                     res = cli.Get("/api/cms/app/pz/" + pzID);
    if (!res || res->status != 200) {
        return false;
    }
    try {
        json j = json::parse(res->body);
        if (!j.contains("code") || j["code"] != 0 || !j.contains("data") || !j["data"].is_object()) {
            return false;
        }
        j = j["data"];
        if (!j.contains("data") || !j["data"].is_object()) {
            return false;
        }
        j = j["data"];
        attrImport(this, j);
        return true;
    } catch (...) {
        return false;
    }
}

bool Character::attrImportFromBackup(const ChAttr &attr) {
    chAttr = attr;
    return true;
}
