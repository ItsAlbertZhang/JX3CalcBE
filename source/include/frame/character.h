#ifndef MAIN_FRAME_CHARACTER_H_
#define MAIN_FRAME_CHARACTER_H_

#include <unordered_map>

namespace ns_frame {

/**
 * @brief Character 类
 * @note 用于表示游戏中的角色.
 * @note 在平时的使用中, 基本上更多使用此类提供的接口, 而不直接操作此类的成员变量的方法.
 */
class Character {
public:
    Character(); // 构造函数

    // ---------- 一些比较确定的实现 ----------

    int nCharacterID;      // 角色 ID
    bool isPlayer = false; // 是否为玩家

    void LearnSkill(int skillID, int skillLevel);
    void CastSkill(int skillID, int skillLevel);
    void AddBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel);

    // ---------- 一些连蒙带猜的实现 ----------

    int nTargetID;                             // 目标 ID
    std::unordered_map<int, int> skillLearned; // 已学习技能列表. key 为技能 ID, value 为技能等级

    //  ---------- 不被 Attribute 包含的属性, 可能被 lua 调用, 以 "n" 开头而非 "at" ----------

    int nSpunkToSolarAndLunarAttackPowerCof = 0;    // 元气转换为阳性和阴性内功攻击的系数
    int nSpunkToSolarAndLunarCriticalStrikeCof = 0; // 元气转换为阳性和阴性内功会心的系数

    int nMaxSunEnergy = 0;  // 最大日灵
    int nMaxMoonEnergy = 0; // 最大月魂

private:
    static int nextCharacterID;
};

class Player : public Character {
};

class NPC : public Character {
};

/**
 * @brief Attribute 类
 * @note 作为 Character 类的一个属性, 其作用是封装, 避免 Character 类过于臃肿.
 */
class Attribute {
public:
    int level = 120;

    int atBasePotentialAdd = 0;       // 所有主属性
    int atVitalityBase = 0;           // 体质
    int atStrengthBase = 0;           // 力道
    int atAgilityBase = 0;            // 身法
    int atSpiritBase = 0;             // 根骨
    int atSpunkBase = 0;              // 元气
    int atVitalityBasePercentAdd = 0; // 体质%
    int atStrengthBasePercentAdd = 0; // 力道%
    int atAgilityBasePercentAdd = 0;  // 身法%
    int atSpiritBasePercentAdd = 0;   // 根骨%
    int atSpunkBasePercentAdd = 0;    // 元气%

    int atPhysicsAttackPowerBase = 0;    // 外功基础攻击
    int atMagicAttackPowerBase = 0;      // 内功基础攻击
    int atSolarAttackPowerBase = 0;      // 阳性内功基础攻击
    int atLunarAttackPowerBase = 0;      // 阴性内功基础攻击
    int atNeutralAttackPowerBase = 0;    // 混元内功基础攻击
    int atPoisonAttackPowerBase = 0;     // 毒性内功基础攻击
    int atPhysicsAttackPowerPercent = 0; // 外功基础攻击提升1024分数
    int atMagicAttackPowerPercent = 0;   // 内功基础攻击提升1024分数
    int atSolarAttackPowerPercent = 0;   // 阳性内功基础攻击提升1024分数
    int atLunarAttackPowerPercent = 0;   // 阴性内功基础攻击提升1024分数
    int atNeutralAttackPowerPercent = 0; // 混元内功基础攻击提升1024分数
    int atPoisonAttackPowerPercent = 0;  // 毒性内功基础攻击提升1024分数
    int atTherapyPowerBase = 0;          // 治疗量
    int atTherapyPowerPercent = 0;       // 治疗量%

    int atAllTypeCriticalStrike = 0;         // 全会心等级
    int atPhysicsCriticalStrike = 0;         // 外功会心等级
    int atMagicCriticalStrike = 0;           // 内功会心等级
    int atSolarCriticalStrike = 0;           // 阳性内功会心等级
    int atLunarCriticalStrike = 0;           // 阴性内功会心等级
    int atNeutralCriticalStrike = 0;         // 混元内功会心等级
    int atPoisonCriticalStrike = 0;          // 毒性内功会心等级
    int atPhysicsCriticalStrikeBaseRate = 0; // 外功额外会心万分数
    int atSolarCriticalStrikeBaseRate = 0;   // 阳性内功额外会心万分数
    int atLunarCriticalStrikeBaseRate = 0;   // 阴性内功额外会心万分数
    int atNeutralCriticalStrikeBaseRate = 0; // 混元内功额外会心万分数
    int atPoisonCriticalStrikeBaseRate = 0;  // 毒性内功额外会心万分数

    int atAllTypeCriticalDamagePowerBase = 0;            // 全会心效果等级
    int atPhysicsCriticalDamagePowerBase = 0;            // 外功会心效果等级
    int atMagicCriticalDamagePowerBase = 0;              // 内功会心效果等级
    int atPhysicsCriticalDamagePowerBaseKiloNumRate = 0; // 外功额外会心效果1024分数
    int atMagicCriticalDamagePowerBaseKiloNumRate = 0;   // 内功额外会心效果1024分数
    int atSolarCriticalDamagePowerBaseKiloNumRate = 0;   // 阳性内功额外会心效果1024分数
    int atLunarCriticalDamagePowerBaseKiloNumRate = 0;   // 阴性内功额外会心效果1024分数
    int atNeutralCriticalDamagePowerBaseKiloNumRate = 0; // 混元内功额外会心效果1024分数
    int atPoisonCriticalDamagePowerBaseKiloNumRate = 0;  // 毒性内功额外会心效果1024分数

    int atPhysicsOvercomeBase = 0;    // 外功基础破防等级
    int atMagicOvercome = 0;          // 内功基础破防等级
    int atSolarOvercomeBase = 0;      // 阳性内功破防等级
    int atLunarOvercomeBase = 0;      // 阴性内功破防等级
    int atNeutralOvercomeBase = 0;    // 混元内功破防等级
    int atPoisonOvercomeBase = 0;     // 毒性内功破防等级
    int atPhysicsOvercomePercent = 0; // 外功基础破防等级提升1024分数
    int atSolarOvercomePercent = 0;   // 阳性内功基础破防等级提升1024分数
    int atLunarOvercomePercent = 0;   // 阴性内功基础破防等级提升1024分数
    int atNeutralOvercomePercent = 0; // 混元内功基础破防等级提升1024分数
    int atPoisonOvercomePercent = 0;  // 毒性内功基础破防等级提升1024分数

    int atSurplusValueBase = 0; // 破招值
    int atStrainBase = 0;       // 无双等级
    int atStrainRate = 0;       // 无双等级提升1024分数
    int atStrainPercent = 0;    // 额外无双1024分数

    int atHasteBase = 0;                  // 基础加速等级
    int atHasteBasePercentAdd = 0;        // 额外加速1024分数
    int atUnlimitHasteBasePercentAdd = 0; // 突破上限加速1024分数

    int atMaxLifeBase = 0;            // 最大气血值
    int atMaxLifePercentAdd = 0;      // 最大气血值%
    int atMaxLifeAdditional = 0;      // 额外气血值
    int atFinalMaxLifeAddPercent = 0; // 最终气血值%

    int atPhysicsShieldBase = 0;         // 外功基础防御等级
    int atPhysicsShieldPercent = 0;      // 外功基础防御等级提升1024分数
    int atPhysicsShieldAdditional = 0;   // 额外外功防御等级
    int atMagicShield = 0;               // 内功基础防御等级
    int atSolarMagicShieldBase = 0;      // 阳性内功防御等级
    int atLunarMagicShieldBase = 0;      // 阴性内功防御等级
    int atNeutralMagicShieldBase = 0;    // 混元内功防御等级
    int atPoisonMagicShieldBase = 0;     // 毒性内功防御等级
    int atSolarMagicShieldPercent = 0;   // 阳性内功基础防御提升1024分数
    int atLunarMagicShieldPercent = 0;   // 阴性内功基础防御提升1024分数
    int atNeutralMagicShieldPercent = 0; // 混元内功基础防御提升1024分数
    int atPoisonMagicShieldPercent = 0;  // 毒性内功基础防御提升1024分数

    int atDodge = 0;             // 闪避等级
    int atDodgeBaseRate = 0;     // 闪避%
    int atParryBase = 0;         // 招架等级
    int atParryPercent = 0;      // 招架等级%
    int atParryBaseRate = 0;     // 招架%
    int atParryValueBase = 0;    // 拆招值
    int atParryValuePercent = 0; // 拆招值%

    int atGlobalResistPercent = 0;       // 通用减伤
    int atPhysicsResistPercent = 0;      // 外功通用减伤%
    int atSolarMagicResistPercent = 0;   // 阳性内功通用减伤%
    int atLunarMagicResistPercent = 0;   // 阴性内功通用减伤%
    int atNeutralMagicResistPercent = 0; // 混元内功通用减伤%
    int atPoisonMagicResistPercent = 0;  // 毒性内功通用减伤%

    int atPhysicsDamageCoefficient = 0; // 物理易伤1024分数
    int atSolarDamageCoefficient = 0;   // 阳性内功易伤1024分数
    int atLunarDamageCoefficient = 0;   // 阴性内功易伤1024分数
    int atNeutralDamageCoefficient = 0; // 混元内功易伤1024分数
    int atPoisonDamageCoefficient = 0;  // 毒性内功易伤1024分数

    int atToughnessBase = 0;     // 御劲等级
    int atToughnessPercent = 0;  // 御劲等级%
    int atToughnessBaseRate = 0; // 御劲%

    int atDecriticalDamagePowerBase = 0;            // 化劲等级
    int atDecriticalDamagePowerPercent = 0;         // 化劲等级%
    int atDecriticalDamagePowerBaseKiloNumRate = 0; // 化劲%

    int atMeleeWeaponDamageBase = 0; // 武器伤害
    int atMeleeWeaponDamageRand = 0; // 武器伤害浮动

    int atAllDamageAddPercent = 0;      // 造成的全伤害和治疗效果提升1024分数
    int atAllMagicDamageAddPercent = 0; // 造成的内功伤害和治疗效果提升1024分数
    int atAddDamageByDstMoveState = 0;  // 根据目标移动状态造成的伤害提升1024分数

    int atAllShieldIgnorePercent = 0;  // 无视防御1024分数
    int atActiveThreatCoefficient = 0; // 仇恨提升1024分数
    int atDstNpcDamageCoefficient = 0; // 非侠士伤害

    int atPhysicsAttackPowerAdd = 0; // 外功额外攻击
    int atSolarAttackPowerAdd = 0;   // 阳性内功额外攻击
    int atLunarAttackPowerAdd = 0;   // 阴性内功额外攻击
    int atNeutralAttackPowerAdd = 0; // 混元内功额外攻击
    int atPoisonAttackPowerAdd = 0;  // 毒性内功额外攻击

    int atPhysicsCriticalStrikeAdd = 0; // 外功额外会心等级
    int atSolarCriticalStrikeAdd = 0;   // 阳性内功额外会心等级
    int atLunarCriticalStrikeAdd = 0;   // 阴性内功额外会心等级
    int atNeutralCriticalStrikeAdd = 0; // 混元内功额外会心等级
    int atPoisonCriticalStrikeAdd = 0;  // 毒性内功额外会心等级

    int atPhysicsOvercomeAdd = 0; // 外功额外破防等级
    int atSolarOvercomeAdd = 0;   // 阳性内功额外破防等级
    int atLunarOvercomeAdd = 0;   // 阴性内功额外破防等级
    int atNeutralOvercomeAdd = 0; // 混元内功额外破防等级
    int atPoisonOvercomeAdd = 0;  // 毒性内功额外破防等级

    int getPhysicsAttackPower(); // 获取外功攻击
    int getSolarAttackPower();   // 获取阳性内功攻击
    int getLunarAttackPower();   // 获取阴性内功攻击
    int getNeutralAttackPower(); // 获取混元内功攻击
    int getPoisonAttackPower();  // 获取毒性内功攻击

    int getPhysicsCriticalStrike(); // 获取外功会心. 结果为万分数, 这符合游戏内的实际原理.
    int getSolarCriticalStrike();   // 获取阳性内功会心. 结果为万分数, 这符合游戏内的实际原理.
    int getLunarCriticalStrike();   // 获取阴性内功会心. 结果为万分数, 这符合游戏内的实际原理.
    int getNeutralCriticalStrike(); // 获取混元内功会心. 结果为万分数, 这符合游戏内的实际原理.
    int getPoisonCriticalStrike();  // 获取毒性内功会心. 结果为万分数, 这符合游戏内的实际原理.

    int getPhysicsCriticalDamagePower(); // 获取外功会心效果. 结果为一个1024分数, 且最大为 1280 (对应 300% 会心效果), 这符合游戏内的实际原理.
    int getSolarCriticalDamagePower();   // 获取阳性内功会心效果. 结果为一个1024分数, 且最大为 1280 (对应 300% 会心效果), 这符合游戏内的实际原理.
    int getLunarCriticalDamagePower();   // 获取阴性内功会心效果. 结果为一个1024分数, 且最大为 1280 (对应 300% 会心效果), 这符合游戏内的实际原理.
    int getNeutralCriticalDamagePower(); // 获取混元内功会心效果. 结果为一个1024分数, 且最大为 1280 (对应 300% 会心效果), 这符合游戏内的实际原理.
    int getPoisonCriticalDamagePower();  // 获取毒性内功会心效果. 结果为一个1024分数, 且最大为 1280 (对应 300% 会心效果), 这符合游戏内的实际原理.

    int getPhysicsOvercome(); // 获取外功破防. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getSolarOvercome();   // 获取阳性内功破防. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getLunarOvercome();   // 获取阴性内功破防. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getNeutralOvercome(); // 获取混元内功破防. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getPoisonOvercome();  // 获取毒性内功破防. 结果为一个1024分数, 这符合游戏内的实际原理.

    int getPhysicsShield(); // 获取外功防御. 结果为一个1024分数, 且最大为 768, 这符合游戏内的实际原理. 注意, 结果并非最终防御! (还需要计算伤害来源的无视防御效果)
    int getSolarShield();   // 获取阳性内功防御. 结果为一个1024分数, 且最大为 768, 这符合游戏内的实际原理. 注意, 结果并非最终防御! (还需要计算伤害来源的无视防御效果)
    int getLunarShield();   // 获取阴性内功防御. 结果为一个1024分数, 且最大为 768, 这符合游戏内的实际原理. 注意, 结果并非最终防御! (还需要计算伤害来源的无视防御效果)
    int getNeutralShield(); // 获取混元内功防御. 结果为一个1024分数, 且最大为 768, 这符合游戏内的实际原理. 注意, 结果并非最终防御! (还需要计算伤害来源的无视防御效果)
    int getPoisonShield();  // 获取毒性内功防御. 结果为一个1024分数, 且最大为 768, 这符合游戏内的实际原理. 注意, 结果并非最终防御! (还需要计算伤害来源的无视防御效果)

    int getStrain(); // 获取无双. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getHaste();  // 获取加速. 结果为一个1024分数, 这符合游戏内的实际原理.

    int getPhysicsDamageAddPercent(); // 获取外功伤害和治疗效果提升. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getSolarDamageAddPercent();   // 获取阳性内功伤害和治疗效果提升. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getLunarDamageAddPercent();   // 获取阴性内功伤害和治疗效果提升. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getNeutralDamageAddPercent(); // 获取混元内功伤害和治疗效果提升. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getPoisonDamageAddPercent();  // 获取毒性内功伤害和治疗效果提升. 结果为一个1024分数, 这符合游戏内的实际原理.

private:
    int get_attack_power(int attribute_type);
    int get_critical_strike(int attribute_type);
    int get_critical_damage_power(int attribute_type);
    int get_overcome(int attribute_type);
    int get_shield(int attribute_type);
    int get_damage_add_percent(int attribute_type);
};

} // namespace ns_frame

#endif // MAIN_FRAME_CHARACTER_H_