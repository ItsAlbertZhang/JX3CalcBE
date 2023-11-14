#pragma once

#include <cstring>

namespace jx3calc {
namespace frame {
/**
 * @brief ChAttr 类
 * @note 从某种意义上来说, 这个类不应当存在, 其下的所有成员变量和函数应当直接放在 Character 类中.
 * @note 然而, 为了避免 Character 类过于臃肿, 同时方便快照, 此处对其进行了封装.
 * @note 设计依据: 是否常用 ROLLBACK. 如果常用回滚, 则放在本类中; 否则放在 Character 类中.
 * @note 例如, 对于日灵月魂, maxEnergy 为 rollback, 因此放在本类中. 而 currentEnergy 为非 rollback, 因此放在 Character 类中.
 * @warning 注意该类的默认构造函数和拷贝构造函数! 应尽量避免向此类中添加高级数据类型, 如 std::string 等. 如果一定要添加, 请注意拷贝构造函数的实现.
 */
class ChAttr {
public:
    ChAttr() = default;
    ChAttr(const ChAttr &obj) {
        memcpy(this, &obj, sizeof(ChAttr));
    }
    ChAttr &operator=(const ChAttr &obj) {
        memcpy(this, &obj, sizeof(ChAttr));
        return *this;
    }

    int atLevel = 100; // 等级

    int atBasePotentialAdd       = 0; // 所有主属性
    int atVitalityBase           = 0; // 体质
    int atStrengthBase           = 0; // 力道
    int atAgilityBase            = 0; // 身法
    int atSpiritBase             = 0; // 根骨
    int atSpunkBase              = 0; // 元气
    int atVitalityBasePercentAdd = 0; // 体质%
    int atStrengthBasePercentAdd = 0; // 力道%
    int atAgilityBasePercentAdd  = 0; // 身法%
    int atSpiritBasePercentAdd   = 0; // 根骨%
    int atSpunkBasePercentAdd    = 0; // 元气%

    int atPhysicsAttackPowerBase    = 0; // 外功基础攻击
    int atMagicAttackPowerBase      = 0; // 内功基础攻击
    int atSolarAttackPowerBase      = 0; // 阳性内功基础攻击
    int atLunarAttackPowerBase      = 0; // 阴性内功基础攻击
    int atNeutralAttackPowerBase    = 0; // 混元内功基础攻击
    int atPoisonAttackPowerBase     = 0; // 毒性内功基础攻击
    int atPhysicsAttackPowerPercent = 0; // 外功基础攻击提升1024分数
    int atMagicAttackPowerPercent   = 0; // 内功基础攻击提升1024分数
    int atSolarAttackPowerPercent   = 0; // 阳性内功基础攻击提升1024分数
    int atLunarAttackPowerPercent   = 0; // 阴性内功基础攻击提升1024分数
    int atNeutralAttackPowerPercent = 0; // 混元内功基础攻击提升1024分数
    int atPoisonAttackPowerPercent  = 0; // 毒性内功基础攻击提升1024分数
    int atTherapyPowerBase          = 0; // 治疗量
    int atTherapyPowerPercent       = 0; // 治疗量%

    int atAllTypeCriticalStrike         = 0; // 全会心等级
    int atPhysicsCriticalStrike         = 0; // 外功会心等级
    int atMagicCriticalStrike           = 0; // 内功会心等级
    int atSolarCriticalStrike           = 0; // 阳性内功会心等级
    int atLunarCriticalStrike           = 0; // 阴性内功会心等级
    int atNeutralCriticalStrike         = 0; // 混元内功会心等级
    int atPoisonCriticalStrike          = 0; // 毒性内功会心等级
    int atPhysicsCriticalStrikeBaseRate = 0; // 外功额外会心万分数
    int atSolarCriticalStrikeBaseRate   = 0; // 阳性内功额外会心万分数
    int atLunarCriticalStrikeBaseRate   = 0; // 阴性内功额外会心万分数
    int atNeutralCriticalStrikeBaseRate = 0; // 混元内功额外会心万分数
    int atPoisonCriticalStrikeBaseRate  = 0; // 毒性内功额外会心万分数

    int atAllTypeCriticalDamagePowerBase            = 0; // 全会心效果等级
    int atPhysicsCriticalDamagePowerBase            = 0; // 外功会心效果等级
    int atMagicCriticalDamagePowerBase              = 0; // 内功会心效果等级
    int atSolarCriticalDamagePowerBase              = 0; // 阳性内功会心效果等级
    int atLunarCriticalDamagePowerBase              = 0; // 阴性内功会心效果等级
    int atNeutralCriticalDamagePowerBase            = 0; // 混元内功会心效果等级
    int atPoisonCriticalDamagePowerBase             = 0; // 毒性内功会心效果等级
    int atPhysicsCriticalDamagePowerBaseKiloNumRate = 0; // 外功额外会心效果1024分数
    int atMagicCriticalDamagePowerBaseKiloNumRate   = 0; // 内功额外会心效果1024分数
    int atSolarCriticalDamagePowerBaseKiloNumRate   = 0; // 阳性内功额外会心效果1024分数
    int atLunarCriticalDamagePowerBaseKiloNumRate   = 0; // 阴性内功额外会心效果1024分数
    int atNeutralCriticalDamagePowerBaseKiloNumRate = 0; // 混元内功额外会心效果1024分数
    int atPoisonCriticalDamagePowerBaseKiloNumRate  = 0; // 毒性内功额外会心效果1024分数

    int atPhysicsOvercomeBase    = 0; // 外功基础破防等级
    int atMagicOvercome          = 0; // 内功基础破防等级
    int atSolarOvercomeBase      = 0; // 阳性内功破防等级
    int atLunarOvercomeBase      = 0; // 阴性内功破防等级
    int atNeutralOvercomeBase    = 0; // 混元内功破防等级
    int atPoisonOvercomeBase     = 0; // 毒性内功破防等级
    int atPhysicsOvercomePercent = 0; // 外功基础破防等级提升1024分数
    int atSolarOvercomePercent   = 0; // 阳性内功基础破防等级提升1024分数
    int atLunarOvercomePercent   = 0; // 阴性内功基础破防等级提升1024分数
    int atNeutralOvercomePercent = 0; // 混元内功基础破防等级提升1024分数
    int atPoisonOvercomePercent  = 0; // 毒性内功基础破防等级提升1024分数

    int atSurplusValueBase = 0; // 破招值
    int atStrainBase       = 0; // 无双等级
    int atStrainRate       = 0; // 无双等级提升1024分数
    int atStrainPercent    = 0; // 额外无双1024分数

    int atHasteBase                  = 0; // 基础加速等级
    int atHasteBasePercentAdd        = 0; // 额外加速1024分数
    int atUnlimitHasteBasePercentAdd = 0; // 突破上限加速1024分数

    int atMaxLifeBase            = 0; // 最大气血值
    int atMaxLifePercentAdd      = 0; // 最大气血值%
    int atMaxLifeAdditional      = 0; // 额外气血值
    int atFinalMaxLifeAddPercent = 0; // 最终气血值%

    int atPhysicsShieldBase         = 0; // 外功基础防御等级
    int atPhysicsShieldPercent      = 0; // 外功基础防御等级提升1024分数
    int atPhysicsShieldAdditional   = 0; // 额外外功防御等级
    int atMagicShield               = 0; // 内功基础防御等级
    int atSolarMagicShieldBase      = 0; // 阳性内功防御等级
    int atLunarMagicShieldBase      = 0; // 阴性内功防御等级
    int atNeutralMagicShieldBase    = 0; // 混元内功防御等级
    int atPoisonMagicShieldBase     = 0; // 毒性内功防御等级
    int atSolarMagicShieldPercent   = 0; // 阳性内功基础防御提升1024分数
    int atLunarMagicShieldPercent   = 0; // 阴性内功基础防御提升1024分数
    int atNeutralMagicShieldPercent = 0; // 混元内功基础防御提升1024分数
    int atPoisonMagicShieldPercent  = 0; // 毒性内功基础防御提升1024分数

    int atDodge             = 0; // 闪避等级
    int atDodgeBaseRate     = 0; // 闪避%
    int atParryBase         = 0; // 招架等级
    int atParryPercent      = 0; // 招架等级%
    int atParryBaseRate     = 0; // 招架%
    int atParryValueBase    = 0; // 拆招值
    int atParryValuePercent = 0; // 拆招值%

    int atGlobalResistPercent       = 0; // 通用减伤
    int atPhysicsResistPercent      = 0; // 外功通用减伤%
    int atSolarMagicResistPercent   = 0; // 阳性内功通用减伤%
    int atLunarMagicResistPercent   = 0; // 阴性内功通用减伤%
    int atNeutralMagicResistPercent = 0; // 混元内功通用减伤%
    int atPoisonMagicResistPercent  = 0; // 毒性内功通用减伤%

    int atPhysicsDamageCoefficient = 0; // 物理易伤1024分数
    int atSolarDamageCoefficient   = 0; // 阳性内功易伤1024分数
    int atLunarDamageCoefficient   = 0; // 阴性内功易伤1024分数
    int atNeutralDamageCoefficient = 0; // 混元内功易伤1024分数
    int atPoisonDamageCoefficient  = 0; // 毒性内功易伤1024分数

    int atToughnessBase     = 0; // 御劲等级
    int atToughnessPercent  = 0; // 御劲等级%
    int atToughnessBaseRate = 0; // 御劲%

    int atDecriticalDamagePowerBase            = 0; // 化劲等级
    int atDecriticalDamagePowerPercent         = 0; // 化劲等级%
    int atDecriticalDamagePowerBaseKiloNumRate = 0; // 化劲%

    int atMeleeWeaponDamageBase = 0; // 武器伤害
    int atMeleeWeaponDamageRand = 0; // 武器伤害浮动

    int atAllDamageAddPercent        = 0; // 造成的全伤害和治疗效果提升1024分数
    int atAllPhysicsDamageAddPercent = 0; // 造成的外功伤害和治疗效果提升1024分数
    int atAllMagicDamageAddPercent   = 0; // 造成的内功伤害和治疗效果提升1024分数

    int atAllShieldIgnorePercent  = 0; // 无视防御1024分数
    int atActiveThreatCoefficient = 0; // 仇恨提升1024分数

    int atPhysicsAttackPowerAdd = 0; // 外功额外攻击
    int atSolarAttackPowerAdd   = 0; // 阳性内功额外攻击
    int atLunarAttackPowerAdd   = 0; // 阴性内功额外攻击
    int atNeutralAttackPowerAdd = 0; // 混元内功额外攻击
    int atPoisonAttackPowerAdd  = 0; // 毒性内功额外攻击

    int atPhysicsCriticalStrikeAdd = 0; // 外功额外会心等级
    int atSolarCriticalStrikeAdd   = 0; // 阳性内功额外会心等级
    int atLunarCriticalStrikeAdd   = 0; // 阴性内功额外会心等级
    int atNeutralCriticalStrikeAdd = 0; // 混元内功额外会心等级
    int atPoisonCriticalStrikeAdd  = 0; // 毒性内功额外会心等级

    int atPhysicsOvercomeAdd = 0; // 外功额外破防等级
    int atSolarOvercomeAdd   = 0; // 阳性内功额外破防等级
    int atLunarOvercomeAdd   = 0; // 阴性内功额外破防等级
    int atNeutralOvercomeAdd = 0; // 混元内功额外破防等级
    int atPoisonOvercomeAdd  = 0; // 毒性内功额外破防等级

    int atBeTherapyCoefficient = 0; // 受到治疗效果提升1024分数

    int getVitality() const; // 体质
    int getStrength() const; // 力道
    int getAgility() const;  // 身法
    int getSpirit() const;   // 根骨
    int getSpunk() const;    // 元气

    int getPhysicsAttackPower() const; // 获取外功攻击
    int getSolarAttackPower() const;   // 获取阳性内功攻击
    int getLunarAttackPower() const;   // 获取阴性内功攻击
    int getNeutralAttackPower() const; // 获取混元内功攻击
    int getPoisonAttackPower() const;  // 获取毒性内功攻击

    int getPhysicsCriticalStrikeValue() const; // 获取外功会心等级. 结果为数值.
    int getSolarCriticalStrikeValue() const;   // 获取阳性内功会心等级. 结果为数值.
    int getLunarCriticalStrikeValue() const;   // 获取阴性内功会心等级. 结果为数值.
    int getNeutralCriticalStrikeValue() const; // 获取混元内功会心等级. 结果为数值.
    int getPoisonCriticalStrikeValue() const;  // 获取毒性内功会心等级. 结果为数值.

    int getPhysicsCriticalStrike() const; // 获取外功会心. 结果为万分数, 这符合游戏内的实际原理.
    int getSolarCriticalStrike() const;   // 获取阳性内功会心. 结果为万分数, 这符合游戏内的实际原理.
    int getLunarCriticalStrike() const;   // 获取阴性内功会心. 结果为万分数, 这符合游戏内的实际原理.
    int getNeutralCriticalStrike() const; // 获取混元内功会心. 结果为万分数, 这符合游戏内的实际原理.
    int getPoisonCriticalStrike() const;  // 获取毒性内功会心. 结果为万分数, 这符合游戏内的实际原理.

    int getPhysicsCriticalDamagePower() const; // 获取外功会心效果. 结果为一个1024分数, 且最大为 1280 (对应 300% 会心效果), 这符合游戏内的实际原理.
    int getSolarCriticalDamagePower() const;   // 获取阳性内功会心效果. 结果为一个1024分数, 且最大为 1280 (对应 300% 会心效果), 这符合游戏内的实际原理.
    int getLunarCriticalDamagePower() const;   // 获取阴性内功会心效果. 结果为一个1024分数, 且最大为 1280 (对应 300% 会心效果), 这符合游戏内的实际原理.
    int getNeutralCriticalDamagePower() const; // 获取混元内功会心效果. 结果为一个1024分数, 且最大为 1280 (对应 300% 会心效果), 这符合游戏内的实际原理.
    int getPoisonCriticalDamagePower() const;  // 获取毒性内功会心效果. 结果为一个1024分数, 且最大为 1280 (对应 300% 会心效果), 这符合游戏内的实际原理.

    int getPhysicsOvercome() const; // 获取外功破防. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getSolarOvercome() const;   // 获取阳性内功破防. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getLunarOvercome() const;   // 获取阴性内功破防. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getNeutralOvercome() const; // 获取混元内功破防. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getPoisonOvercome() const;  // 获取毒性内功破防. 结果为一个1024分数, 这符合游戏内的实际原理.

    int getPhysicsShield(int srcShieldIgnorePercent) const; // 获取外功防御. 结果为一个1024分数, 且最大为 768, 这符合游戏内的实际原理.
    int getSolarShield(int srcShieldIgnorePercent) const;   // 获取阳性内功防御. 结果为一个1024分数, 且最大为 768, 这符合游戏内的实际原理.
    int getLunarShield(int srcShieldIgnorePercent) const;   // 获取阴性内功防御. 结果为一个1024分数, 且最大为 768, 这符合游戏内的实际原理.
    int getNeutralShield(int srcShieldIgnorePercent) const; // 获取混元内功防御. 结果为一个1024分数, 且最大为 768, 这符合游戏内的实际原理.
    int getPoisonShield(int srcShieldIgnorePercent) const;  // 获取毒性内功防御. 结果为一个1024分数, 且最大为 768, 这符合游戏内的实际原理.

    int getStrain() const;  // 获取无双. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getHaste() const;   // 获取加速. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getSurplus() const; // 获取破招. 结果为一个数值, 将其与技能的破招系数相乘可得原始伤害.

    int getPhysicsDamageAddPercent() const; // 获取外功伤害和治疗效果提升. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getSolarDamageAddPercent() const;   // 获取阳性内功伤害和治疗效果提升. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getLunarDamageAddPercent() const;   // 获取阴性内功伤害和治疗效果提升. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getNeutralDamageAddPercent() const; // 获取混元内功伤害和治疗效果提升. 结果为一个1024分数, 这符合游戏内的实际原理.
    int getPoisonDamageAddPercent() const;  // 获取毒性内功伤害和治疗效果提升. 结果为一个1024分数, 这符合游戏内的实际原理.

    int atDstNpcDamageCoefficient = 0; // 非侠士伤害提升1024分数
    int atGlobalDamageFactor      = 0; // 全局伤害系数, 通常用于调整破招伤害
    int atAddDamageByDstMoveState = 0; // 根据目标移动状态造成的伤害提升1024分数, 出现于: 明教/秘笈/明教_秘籍_烈日斩_静止目标加伤害.lua

    int atSpunkToSolarAndLunarAttackPowerCof    = 0; // 元气转换为阳性和阴性内功攻击的系数
    int atSpunkToSolarAndLunarCriticalStrikeCof = 0; // 元气转换为阳性和阴性内功会心的系数
    // ... 其余未出现的属性暂时省略

    int atMaxSunEnergy  = 0; // 最大日灵
    int atMaxMoonEnergy = 0; // 最大月魂
};
} // namespace frame
} // namespace jx3calc
