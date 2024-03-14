#include "frame/character/derived/player.h"
#include <format>
#include <vector>

using namespace ns_frame;

std::shared_ptr<CustomLua> CustomLua::get(const std::string &script) {
    if (!mapCustomLua.contains(script)) {
        mapCustomLua.emplace(script, std::make_shared<CustomLua>(script));
    }
    return mapCustomLua.at(script);
}

void CustomLua::cancel(const std::string &script) {
    if (mapCustomLua.contains(script)) {
        mapCustomLua.erase(script);
    }
}

static std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string              token;
    std::istringstream       tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

static std::string conditionConvert(const std::string &condition) {
    // condition: buff:日月同辉=3&nobuff:诛邪镇魔|sun<20
    std::string res = "(";
    const char *p   = condition.c_str();
    while (*p != '&' && *p != '|' && *p != '\0')
        p++;
    const std::string sub = condition.substr(0, p - condition.c_str());
    if (sub.starts_with("buff:")) {
        res += "player:macroBuff(\"";
        const char *o = sub.c_str() + 5;
        while (*o != '=' && *o != '<' && *o != '>' && *o != '\0')
            o++;
        res += sub.substr(5, o - sub.c_str() - 5) + "\")";
        if (*o != '\0') {
            res += *o;
        }
    } else if (sub.starts_with("nobuff:")) {
        res += "player:macroNoBuff(\"";
        res += sub.substr(7);
    } else if (sub.starts_with("bufftime:")) {
        res += "player:macroBufftime(\"";
        const char *o = sub.c_str() + 9;
        while (*o != '=' && *o != '<' && *o != '>' && *o != '\0')
            o++;
        res += sub.substr(9, o - sub.c_str() - 9) + "\")";
        if (*o != '\0') {
            res += *o;
        }
    } else if (sub.starts_with("tbuff:")) {
        res += "player:macroTBuff(\"";
        const char *o = sub.c_str() + 6;
        while (*o != '=' && *o != '<' && *o != '>' && *o != '\0')
            o++;
        res += sub.substr(6, o - sub.c_str() - 6) + "\")";
        if (*o != '\0') {
            res += *o;
        }
    } else if (sub.starts_with("tnobuff:")) {
        res += "player:macroTNoBuff(\"";
        res += sub.substr(8);
    } else if (sub.starts_with("tbufftime:")) {
        res += "player:macroTBufftime(\"";
        const char *o = sub.c_str() + 10;
        while (*o != '=' && *o != '<' && *o != '>' && *o != '\0')
            o++;
        res += sub.substr(10, o - sub.c_str() - 10) + "\")";
        if (*o != '\0') {
            res += *o;
        }
    } else if (sub.starts_with("sun")) {
        res += "player.nCurrentSunEnergy" + sub.substr(3) + "00";
    } else if (sub.starts_with("moon")) {
        res += "player.nCurrentMoonEnergy" + sub.substr(4) + "00";
    }
    return res + conditionConvert(condition.substr(p - condition.c_str() + 1)) + ")";
}

std::string CustomLua::parse(std::vector<std::string> macroList) {
    std::string result;
    result += std::format("function Init() end;\nMacroNum = {};\n", macroList.size());
    for (size_t i = 0; i < macroList.size(); i++) {
        result += std::format("function Macro{}(player)\n", i);
        auto lines = split(macroList[i], '\n'); // 将字符串以 \n 为分隔符进行分割
        for (const auto &line : lines) {
            auto words     = split(line, ' '); // 将字符串以空格为分隔符进行分割
            bool condition = words.size() == 3 && words[1].starts_with("[") && words[1].ends_with("]");
            if (words[0] == "/cast") {
                if (condition)
                    result += "    if" + conditionConvert(words[1].substr(1, words[1].size() - 2)) + " then\n";
                result += "        player:macroSkillCast(\"" + words[2] + "\");";
                if (condition)
                    result += "    end\n";
            }
        }
        result += "end\n\n";
    }
    return result;
}

static void constructorBefore(CustomLua *self);
static void constructorAfter(CustomLua *self);

CustomLua::CustomLua(const std::string &script) {
    constructorBefore(this);
    lua.script(script);
    constructorAfter(this);
}

static void constructorBefore(CustomLua *self) {
    self->lua.open_libraries(sol::lib::base);

    auto chAttr = self->lua.new_usertype<ChAttr>("ChAttr");

    chAttr["atVitalityBase"]                   = &ChAttr::atVitalityBase;
    chAttr["atStrengthBase"]                   = &ChAttr::atStrengthBase;
    chAttr["atAgilityBase"]                    = &ChAttr::atAgilityBase;
    chAttr["atSpiritBase"]                     = &ChAttr::atSpiritBase;
    chAttr["atSpunkBase"]                      = &ChAttr::atSpunkBase;
    chAttr["atPhysicsAttackPowerBase"]         = &ChAttr::atPhysicsAttackPowerBase;
    chAttr["atSolarAttackPowerBase"]           = &ChAttr::atSolarAttackPowerBase;
    chAttr["atLunarAttackPowerBase"]           = &ChAttr::atLunarAttackPowerBase;
    chAttr["atNeutralAttackPowerBase"]         = &ChAttr::atNeutralAttackPowerBase;
    chAttr["atPoisonAttackPowerBase"]          = &ChAttr::atPoisonAttackPowerBase;
    chAttr["atTherapyPowerBase"]               = &ChAttr::atTherapyPowerBase;
    chAttr["atPhysicsCriticalStrike"]          = &ChAttr::atPhysicsCriticalStrike;
    chAttr["atSolarCriticalStrike"]            = &ChAttr::atSolarCriticalStrike;
    chAttr["atLunarCriticalStrike"]            = &ChAttr::atLunarCriticalStrike;
    chAttr["atNeutralCriticalStrike"]          = &ChAttr::atNeutralCriticalStrike;
    chAttr["atPoisonCriticalStrike"]           = &ChAttr::atPoisonCriticalStrike;
    chAttr["atPhysicsCriticalDamagePowerBase"] = &ChAttr::atPhysicsCriticalDamagePowerBase;
    chAttr["atSolarCriticalDamagePowerBase"]   = &ChAttr::atSolarCriticalDamagePowerBase;
    chAttr["atLunarCriticalDamagePowerBase"]   = &ChAttr::atLunarCriticalDamagePowerBase;
    chAttr["atNeutralCriticalDamagePowerBase"] = &ChAttr::atNeutralCriticalDamagePowerBase;
    chAttr["atPoisonCriticalDamagePowerBase"]  = &ChAttr::atPoisonCriticalDamagePowerBase;
    chAttr["atPhysicsOvercomeBase"]            = &ChAttr::atPhysicsOvercomeBase;
    chAttr["atSolarOvercomeBase"]              = &ChAttr::atSolarOvercomeBase;
    chAttr["atLunarOvercomeBase"]              = &ChAttr::atLunarOvercomeBase;
    chAttr["atNeutralOvercomeBase"]            = &ChAttr::atNeutralOvercomeBase;
    chAttr["atPoisonOvercomeBase"]             = &ChAttr::atPoisonOvercomeBase;
    chAttr["atSurplusValueBase"]               = &ChAttr::atSurplusValueBase;
    chAttr["atStrainBase"]                     = &ChAttr::atStrainBase;
    chAttr["atHasteBase"]                      = &ChAttr::atHasteBase;
    chAttr["atPhysicsShieldBase"]              = &ChAttr::atPhysicsShieldBase;
    chAttr["atSolarMagicShieldBase"]           = &ChAttr::atSolarMagicShieldBase;
    chAttr["atLunarMagicShieldBase"]           = &ChAttr::atLunarMagicShieldBase;
    chAttr["atNeutralMagicShieldBase"]         = &ChAttr::atNeutralMagicShieldBase;
    chAttr["atPoisonMagicShieldBase"]          = &ChAttr::atPoisonMagicShieldBase;
    chAttr["atDodge"]                          = &ChAttr::atDodge;
    chAttr["atParryBase"]                      = &ChAttr::atParryBase;
    chAttr["atParryValueBase"]                 = &ChAttr::atParryValueBase;
    chAttr["atToughnessBase"]                  = &ChAttr::atToughnessBase;
    chAttr["atDecriticalDamagePowerBase"]      = &ChAttr::atDecriticalDamagePowerBase;
    chAttr["atMeleeWeaponDamageBase"]          = &ChAttr::atMeleeWeaponDamageBase;
    chAttr["atMeleeWeaponDamageRand"]          = &ChAttr::atMeleeWeaponDamageRand;

    auto buff = self->lua.new_usertype<BuffItem>("Buff");

    buff["nLevel"]           = &BuffItem::nLevel;
    buff["nIndex"]           = &BuffItem::nIndex;
    buff["nStackNum"]        = &BuffItem::nStackNum;
    buff["nLeftActiveCount"] = &BuffItem::nLeftActiveCount;
    buff["nNextActiveFrame"] = &BuffItem::nNextActiveFrame;
    buff["nCustomValue"]     = &BuffItem::nCustomValue;

    auto player = self->lua.new_usertype<Player>("Player");

    player["chAttr"]                 = &Player::chAttr;
    player["characterGetSelect"]     = &Player::characterGetSelect;
    player["characterGetTargetID"]   = &Player::characterGetTargetID;
    player["buffExist"]              = &Player::buffExist;
    player["buffGet"]                = &Player::buffGet;
    player["buffGetByOwner"]         = &Player::buffGetByOwner;
    player["buffAdd"]                = &Player::buffAddOptional;
    player["buffDel"]                = &Player::buffDel;
    player["buffDelGroup"]           = &Player::buffDelGroup;
    player["buffDelMultiGroupByID"]  = &Player::buffDelMultiGroupByID;
    player["buffSetLeftActiveCount"] = &Player::buffSetLeftActiveCount;
    player["buffSetNextActiveFrame"] = &Player::buffSetNextActiveFrame;
    player["cooldownClearTime"]      = &Player::cooldownClearTime;
    player["cooldownModify"]         = &Player::cooldownModify;
    player["cooldownReset"]          = &Player::cooldownReset;
    player["skillGetLevel"]          = &Player::skillGetLevel;
    player["skillCast"]              = sol::overload(
        static_cast<void (Character::*)(int, int)>(&Character::skillCast),
        static_cast<void (Character::*)(int, int, int)>(&Character::skillCast),
        static_cast<void (Character::*)(int, int, int, int)>(&Character::skillCast)
    );
    player["skillCastXYZ"]     = &Player::skillCastXYZ;
    player["skillrecipeExist"] = &Player::skillrecipeExist;
    player["sceneGet"]         = &Player::sceneGet;
    player["timerSet"]         = sol::overload(
        static_cast<void (Character::*)(int, std::string, int)>(&Character::timerSet),
        static_cast<void (Character::*)(int, std::string, int, int)>(&Character::timerSet)
    );
    player["otherIsFormationLeader"]         = &Player::otherIsFormationLeader;
    player["otherIsInParty"]                 = &Player::otherIsInParty;
    player["otherGetKungfuMountID"]          = &Player::otherGetKungfuMountID;
    player["otherGetMapID"]                  = &Player::otherGetMapID;
    player["otherClearAllNpcThreat"]         = &Player::otherClearAllNpcThreat;
    player["otherCreatePublicShadow"]        = &Player::otherCreatePublicShadow;
    player["otherDestroyPublicShadow"]       = &Player::otherDestroyPublicShadow;
    player["otherDoAction"]                  = &Player::otherDoAction;
    player["otherPlayPublicShadowAnimation"] = &Player::otherPlayPublicShadowAnimation;
    player["dwID"]                           = &Player::dwID;
    player["nLevel"]                         = &Player::nLevel;
    player["nX"]                             = &Player::nX;
    player["nY"]                             = &Player::nY;
    player["nZ"]                             = &Player::nZ;
    player["nRoleType"]                      = &Player::nRoleType;
    player["nCurrentSunEnergy"]              = &Player::nCurrentSunEnergy;
    player["nCurrentMoonEnergy"]             = &Player::nCurrentMoonEnergy;
    player["nSunPowerValue"]                 = &Player::nSunPowerValue;
    player["nMoonPowerValue"]                = &Player::nMoonPowerValue;
    player["bSurplusAutoCast"]               = &Player::bSurplusAutoCast;
    player["bSurplusAutoReplenish"]          = &Player::bSurplusAutoReplenish;
    player["bFightState"]                    = &Player::bFightState;
    player["fMaxLife64"]                     = &Player::fMaxLife64;
    player["fCurrentLife64"]                 = &Player::fCurrentLife64;
    player["attrImportFromData"]             = &Player::attrImportFromData;
    player["attrImportFromJX3BOX"]           = &Player::attrImportFromJX3BOX;
    player["buffTimeLeftTick"]               = &Player::buffTimeLeftTick;
    player["cast"]                           = &Player::cast;
    player["skillActive"]                    = &Player::skillActive;
    player["skillDeactive"]                  = &Player::skillDeactive;
    player["skillLearn"]                     = &Player::skillLearn;
    player["skillrecipeAdd"]                 = &Player::skillrecipeAdd;
    player["skillrecipeRemove"]              = &Player::skillrecipeRemove;
    player["skilleventAdd"]                  = &Player::skilleventAdd;
    player["skilleventRemove"]               = &Player::skilleventRemove;
    player["dwKungfuID"]                     = &Player::dwKungfuID;
    player["publicCooldownID"]               = &Player::publicCooldownID;
    player["delayBase"]                      = &Player::delayBase;
    player["delayRand"]                      = &Player::delayRand;
    player["delayCustom"]                    = &Player::delayCustom;
    player["macroIdx"]                       = &Player::macroIdx;

    player["macroSkillCast"] = &Player::macroSkillCast;
    player["macroBuff"]      = &Player::macroBuff;
    player["macroNoBuff"]    = &Player::macroNoBuff;
    player["macroBufftime"]  = &Player::macroBufftime;
    player["macroTBuff"]     = &Player::macroTBuff;
    player["macroTNoBuff"]   = &Player::macroTNoBuff;
    player["macroTBufftime"] = &Player::macroTBufftime;
}

static void constructorAfter(CustomLua *self) {
    self->init   = self->lua["Init"];
    int macroNum = self->lua["MacroNum"].get<int>();
    for (int i = 0; i < macroNum; i++) {
        self->macroRuntime.push_back(self->lua["Macro" + std::to_string(i)]);
    }
}
