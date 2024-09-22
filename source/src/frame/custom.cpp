#include "frame/character/player.h"
#include <format>
#include <string>
#include <unordered_map>
#include <vector>

using namespace jx3calc;
using namespace frame;

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

    const char *p = s.c_str(), *q = s.c_str();
    while (*p != '\0') {
        if (*p == delimiter) {
            tokens.emplace_back(q, p - q);
            q = p + 1;
        }
        p++;
    }
    tokens.emplace_back(q, p - q);
    return tokens;
}

static std::string convertRValBuff(const char *start, size_t len) {
    if (0 == len) {
        return ">0";
    }
    return std::string(start, len);
}

static std::string convertRValSunMoon(const char *start, size_t len) {
    if (0 == len) {
        return ">0";
    }
    std::string str {start + 1, len - 1};
    if (str == "sun") {
        str = "player.nCurrentSunEnergy";
    } else if (str == "moon") {
        str = "player.nCurrentMoonEnergy";
    } else {
        try {
            str = std::to_string(std::stoi(str) * 100);
        } catch (...) {
        }
    }
    return std::format("{}{}", *start, str);
}

static std::string convertCondition(const char *cond) {
    // cond: buff:日月同辉=3&nobuff:诛邪镇魔|sun<20
    class CondType {
    public:
        const size_t      len;
        const std::string name;
        std::string (*convert)(const char *start, size_t len);
    };
    static const std::unordered_map<std::string, CondType> condMap = {
        {"buff", {4, "player:macroBuff", convertRValBuff}},
        {"nobuff", {6, "player:macroNoBuff", nullptr}},
        {"bufftime", {8, "player:macroBufftime", nullptr}},
        {"tbuff", {5, "player:macroTBuff", convertRValBuff}},
        {"tnobuff", {7, "player:macroTNoBuff", nullptr}},
        {"tbufftime", {9, "player:macroTBufftime", nullptr}},
        {"sun", {3, "player.nCurrentSunEnergy", convertRValSunMoon}},
        {"moon", {4, "player.nCurrentMoonEnergy", convertRValSunMoon}},
    };
    if (*cond == '\0') {
        return "";
    }
    // 检查条件类型
    const CondType *type = nullptr;
    for (const auto &[key, value] : condMap) {
        if (strncmp(cond, key.c_str(), key.length()) == 0) {
            type = &value;
            break;
        }
    }
    if (type == nullptr) {
        return "";
    }

    // 计算
    const char *end = cond;
    while (*end != '&' && *end != '|' && *end != '\0')
        end++; // end 指向 & 或 | 或 \0
    const char *argument = cond + type->len;
    const char *op       = argument;
    while (op < end && *op != '=' && *op != '<' && *op != '>')
        op++;
    return std::format(
        "({}{}{}{}{}{})",
        type->name,    // 函数调用或成员变量
        argument == op // 如果是带参数的函数调用, 补充参数
            ? ""
            : "(\"" + std::string(argument + 1, op - argument - 1) + "\")",
        *op == '=' ? "=" : "",   // 如果操作符是等号, 补一个等号
        type->convert == nullptr // 操作符和操作符右值
            ? std::string(op, end - op)
            : type->convert(op, end - op),
        *end == '&' ? " and " : (*end == '|' ? " or " : ""), // 与下一个条件的连接符
        convertCondition(end + 1)
    );
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
                if (condition) {
                    result += "    if" + convertCondition(words[1].substr(1, words[1].size() - 2).c_str()) + " then\n";
                    result += "        player:macroSkillCast(\"" + words[2] + "\");\n";
                    result += "    end\n";
                } else {
                    result += "    player:macroSkillCast(\"" + words[1] + "\");\n";
                }
            } else if (words[0] == "/switch") {
                int idx = condition ? 2 : 1;
                try {
                    int newIdx = std::stoi(words[idx]);
                    if (newIdx < 0 || newIdx >= macroList.size()) {
                        throw std::exception();
                    }
                    if (condition) {
                        result += "    if" + convertCondition(words[1].substr(1, words[1].size() - 2).c_str()) + " then\n";
                        result += "        player.macroIdx = " + std::to_string(newIdx) + ";\n";
                        result += "    end\n";
                    } else {
                        result += "        player.macroIdx = " + std::to_string(newIdx) + ";\n";
                    }
                } catch (...) {
                }
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
    self->lua.open_libraries(sol::lib::base, sol::lib::math);

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
    player["characterGetEmployer"]   = &Player::characterGetEmployer;
    player["characterGetSelect"]     = &Player::characterGetSelect;
    player["characterGetTargetID"]   = &Player::characterGetTargetID;
    player["buffExist"]              = &Player::buffExist;
    player["buffGet"]                = &Player::buffGet;
    player["buffGetByOwner"]         = &Player::buffGetByOwner;
    player["buffAdd"]                = &Player::buffAddOptional;
    player["buffDel"]                = &Player::buffDel;
    player["buffDelGroup"]           = &Player::buffDelGroup;
    player["buffDelMultiCount"]      = &Player::buffDelMultiCount;
    player["buffDelMultiGroupByID"]  = &Player::buffDelMultiGroupByID;
    player["buffSetLeftActiveCount"] = &Player::buffSetLeftActiveCount;
    player["buffSetNextActiveFrame"] = &Player::buffSetNextActiveFrame;
    player["cooldownClearTime"]      = &Player::cooldownClearTime;
    player["cooldownModify"]         = static_cast<void (Character::*)(int, int)>(&Character::cooldownModify);
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
    player["nLevel"]                         = sol::property(&Player::nLevel_getter, &Player::nLevel_setter);
    player["attrImportFromData"]             = &Player::attrImportFromData;
    player["attrImportFromJX3BOX"]           = &Player::attrImportFromJX3BOX;
    player["buffTimeLeftTick"]               = &Player::buffTimeLeftTick;
    player["itemAdd"]                        = static_cast<void (Character::*)(int, int)>(&Player::itemAdd);
    player["itemUse"]                        = static_cast<void (Character::*)(int, int)>(&Player::itemUse);
    player["cast"]                           = &Player::cast;
    player["skillActive"]                    = &Player::skillActive;
    player["skillDeactive"]                  = &Player::skillDeactive;
    player["skillLearn"]                     = &Player::skillLearn;
    player["skillrecipeAdd"]                 = &Player::skillrecipeAdd;
    player["skillrecipeRemove"]              = &Player::skillrecipeRemove;
    player["skilleventAdd"]                  = &Player::skilleventAdd;
    player["skilleventRemove"]               = &Player::skilleventRemove;
    player["dwKungfuID"]                     = &Player::kungfuID;
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
    self->fightPrepareAdd = self->lua["Init"];
    int macroNum          = self->lua["MacroNum"].get<int>();
    for (int i = 0; i < macroNum; i++) {
        self->macroRuntime.push_back(self->lua["Macro" + std::to_string(i)]);
    }
}
