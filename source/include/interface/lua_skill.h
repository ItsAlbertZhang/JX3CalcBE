#ifndef MAIN_INTERFACE_SKILL_H_
#define MAIN_INTERFACE_SKILL_H_

#include "frame/skill.h"
#include <string>

namespace ns_interface {

// 技能接口.
class InterfaceSkill {
public:
    int dwLevel = 1;
    double nChannelInterval = 0;
    static thread_local ns_frame::Skill *current_skill;

    /* 需要处理后才能使用的属性 */
    static void AddAttribute_iiii(int a, int b, int c, int d);
    static void AddAttribute_iisi(int a, int b, std::string c, int d);
    static void AddAttribute_iidi(int a, int b, double c, int d);

    /* 直接传入 lua 的属性 */

    // 技能施放Buff需求
    static void AddSlowCheckSelfBuff(int a, int b, int c, int d, int e);    // 需求自身Buff
    static void AddSlowCheckDestBuff(int a, int b, int c, int d, int e);    // 需求目标Buff
    static void AddSlowCheckSelfOwnBuff(int a, int b, int c, int d, int e); // 需求目标Buff
    static void AddSlowCheckDestOwnBuff(int a, int b, int c, int d, int e); // 需求目标Buff
    // 技能施放技能需求
    static void AddCheckSelfLearntSkill(int a, int b, int c); // 检查比较Caster自己已学习的技能ID和等级
    // BUFF相关
    static void BindBuff(int a, int b, int c); // 设置a号位Buff

    // 设置Cool down
    static void SetPublicCoolDown(int a);
    static void SetNormalCoolDown(int a, int b); // 技能普通CD
    static void SetCheckCoolDown(int a, int b);  // 后跳公共CD

    // 经验升级相关
    int dwLevelUpExp = 0;      // 升级经验
    int nExpAddOdds = 1024;    // 技能熟练度增长概率
    int nPlayerLevelLimit = 0; // 角色可以学会该技能所必须达到的最低等级

    // 技能仇恨
    int nBaseThreat = 0;

    // 技能消耗
    int nCostLife = 0;        // 技能消耗生命值
    int nCostMana = 0;        // 技能消耗的内力
    int nCostStamina = 0;     // 技能消耗的体力
    int nCostItemType = 0;    // 技能消耗的物品类型
    int nCostItemIndex = 0;   // 技能消耗的物品索引ID
    int nCostSprintPower = 0; // 技能消耗气力值

    // 聚气相关
    bool bIsAccumulate = false; // 技能是否需要聚气

    // 链状技能相关
    static void SetSubsectionSkill(int a, int b, int c, int d);
    int nChainBranch = 0; // 链状技能分支数
    int nChainDepth = 0;  // 链状技能层数

    // 施放距离
    int nMinRadius = 0; // 技能施放的最小距离
    int nMaxRadius = 0; // 技能施放的最大距离

    // 作用范围
    int nProtectRadius = 0; // 环形和矩形AOE的保护距离，范围内不受伤害
    int nHeight = 0;        // AOE的高度，全高，圆柱体AOE中不填为2倍的nAreaRadius，矩形AOE中不填为nAreaRadius
    int nRectWidth = 0;     // 矩形AOE的宽度，全宽，不填为nAreaRadius
    int nAngleRange = 0;    // 攻击范围的扇形角度范围
    bool bFullAngleInAir = false;
    int nAreaRadius = 0;       // 技能作用半径
    int nTargetCountLimit = 0; // 技能作用目标数量限制,(小于0 代表目标数量不限制)

    // 时间相关
    int nPrepareFrames = 0;  // 吟唱帧数
    int nChannelFrame = 0;   // 通道技持续时间，单位帧数
    int nBulletVelocity = 0; // 子弹速度，单位 点/帧

    // 阵法相关
    bool bIsFormationSkill = false;    // 是否阵眼技能
    int nFormationRange = 0;           // 结阵的范围
    int nLeastFormationPopulation = 2; // 结阵的范围的最少队员数（包括队长）

    // 目标血量需求
    int nTargetLifePercentMin = 0;   // 血量最小值>=
    int nTargetLifePercentMax = 100; // 血量最大值<=

    // 自身血量需求
    int nSelfLifePercentMin = 0;   // 血量最小值>=
    int nSelfLifePercentMax = 100; // 血量最大值<=

    // 打退打断落马相关
    int nBeatBackRate = 1024; // 技能被打退的概率,默认1024
    int nBrokenRate = 1024;   // 技能被打断的概率,默认1024
    int nBreakRate = 0;       // 打断目标施法的概率,基数1024
    int nDismountingRate = 0; // 将目标击落下马几率,基数1024，默认0

    // 武器伤害相关
    int nWeaponDamagePercent = 0; // 武器伤害百分比,对外功伤害有用。填0表示此次外功攻击不计算武器伤害,1024为100%
};

} // namespace ns_interface

#endif // MAIN_INTERFACE_SKILL_H_