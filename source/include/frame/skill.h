#ifndef MAIN_FRAME_SKILL_H_
#define MAIN_FRAME_SKILL_H_

#include <sol2/sol.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace ns_frame {

/**
 * 技能类
 * 注意: 技能是公共资源, 不属于某个角色. 应当使用技能管理类对其统一进行存取.
 */
class Skill {
public:
    // ---------- 数据存放区 ----------
    static thread_local ns_frame::Skill *current_skill; // 当前正在被 lua 操作的技能
    std::unordered_map<std::string, std::string> tab;   // skills.tab 中的数据
    sol::protected_function GetSkillLevelData;          // GetSkillLevelData 函数
    class SkillAttribute {
    public:
        SkillAttribute(int mode, int type, int param1, int param2)
            : mode(mode), type(type), param1(param1), param2(param2) {}
        int mode;
        int type;
        int param1;
        int param2;
    };
    std::vector<SkillAttribute> attributes; // GetSkillLevelData 函数中添加的属性
    class SkillAttributeString {
    public:
        SkillAttributeString(int mode, int type, std::string param1, int param2)
            : mode(mode), type(type), param1(param1), param2(param2) {}
        int mode;
        int type;
        std::string param1;
        int param2;
    };
    std::vector<SkillAttributeString> attributesString; // GetSkillLevelData 函数中添加的属性

    // ---------- 技能等级 ----------
    int dwLevel = 1;

    // ---------- 魔法属性 ----------
    static void AddAttribute_iiii(int a, int b, int c, int d);
    static void AddAttribute_iisi(int a, int b, std::string c, int d);
    static void AddAttribute_iidi(int a, int b, double c, int d);

    // ---------- lua 直接操作的属性 ----------

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
    int nPrepareFrames = 0;      // 吟唱帧数
    double nChannelInterval = 0; // 通道技间隔时间 (实际上为技能系数, 且为 int 类型. 此处使用 double 类型是为了方便 lua 直接操作.)
    int nChannelFrame = 0;       // 通道技持续时间，单位帧数
    int nBulletVelocity = 0;     // 子弹速度，单位 点/帧

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

/**
 * 技能管理类
 * 这是一个静态类, 用于管理技能数据, 不应当被创建实例.
 */
class SkillManager {
public:
    SkillManager() = delete; // 禁止创建实例

    /**
     * 技能缓存数据
     * 同一 ID, 不同 Level 的技能拥有不同的 Skill 实例.
     */
    static thread_local std::unordered_map<int, std::unordered_map<int, Skill>> data;

    /**
     * @brief 获取技能. 若技能存在, 则命中缓存并返回技能数据; 若技能不存在, 则对其进行初始化并返回技能数据.
     * @param skillID
     * @param skillLevel
     * @return Skill&
     */
    static Skill &get(int skillID, int skillLevel);

private:
    /**
     * @brief 初始化技能. 将指定 ID 与 Level 的技能数据存至缓存.
     * @param skillID
     * @param skillLevel
     */
    static void add(int skillID, int skillLevel);
};

} // namespace ns_frame

#endif // MAIN_FRAME_SKILL_H_