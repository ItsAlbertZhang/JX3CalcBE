// 如果你不懂 C++ 语言, 不需要关注开头的部分.
// ----------始----------
#include <string>
using namespace std;
extern class Character;
extern class ChAttr;
extern class BuffItem;
// ----------终----------

// 以下为 C++ 暴露给 lua 的 API. 你可以在 lua 中直接调用它们.

// --------------------分割线--------------------
// Player. 这些属性和方法可以直接通过 lua 函数的参数 player 进行调用.

int    dwKungfuID;         // 当前心法 ID
int    publicCooldownID;   // 当前心法公共 CD ID
int    delayBase;          // 基础延迟 (网络延迟). 单位为 tick, 1024 tick 为 1 秒.
int    delayRand;          // 随机延迟 (按键延迟). 单位为 tick, 1024 tick 为 1 秒.
int    macroIdx;           // 当前宏
ChAttr chAttr;             // 角色属性. 详见下文 ChAttr 部分.
int    dwID;               // JX3Calc 中的角色 ID
int    nLevel;             // 角色等级
int    nCurrentSunEnergy;  // 当前日灵
int    nCurrentMoonEnergy; // 当前月魂
int    nSunPowerValue;     // 满日
int    nMoonPowerValue;    // 满月
int    bFightState;        // 是否处于战斗状态

// 自定义延迟. 单位为 tick, 1024 tick 为 1 秒.
// 当自定义延迟被设定时, 下一次释放宏的时机将是该延迟,
// 而不再是 公共 CD 冷却完成后 + 基础延迟 + 随机延迟
int delayCustom;

// 释放指定 ID 的已学习的主动技能.
// 返回值为技能是否成功释放.
bool cast(int skillID);

// type: 1 为 盔甲, 2 为 饰品, 3 为 武器.
// ID 为装备 ID. 可以通过按住 Ctrl 键并将鼠标放置在主动装备上来获取.
void itemAdd(int type, int ID); // 添加主动装备
void itemUse(int type, int ID); // 使用主动装备

void skillActive(int skillID);                // 激活被动技能
void skillDeactive(int skillID);              // 取消激活被动技能
void skillLearn(int skillID, int skillLevel); // 学习技能

// recipeID 与 recipeLevel 可以通过按住 Ctrl 键并将鼠标放置在秘籍上来获取.
void skillrecipeAdd(int recipeID, int recipeLevel);    // 激活秘籍
void skillrecipeRemove(int recipeID, int recipeLevel); // 移除秘籍

void skilleventAdd(int eventID);    // 激活技能事件
void skilleventRemove(int eventID); // 移除技能事件

Character *characterGetSelect();   // 获取当前选中的角色实例
int        characterGetTargetID(); // 获取当前选中的目标 ID

// 判断是否存在指定的 buff.
// buffLevel 可以为 0, 以代表任意等级
bool buffExist(int buffID, int buffLevel);

// 获取 buff 实例.
// buffLevel 可以为 0, 以代表任意等级.
// 返回的结果可能为 nil, 注意检查返回值.
BuffItem *buffGet(int buffID, int buffLevel); // 获取指定的 buff 实例

// 获取来自指定来源的 buff 实例.
// sourceID 应当为 buff 的施加者 ID.
// 返回的结果可能为 nil, 注意检查返回值.
BuffItem *buffGetByOwner(int buffID, int buffLevel, int sourceID);

// 获取 buff 剩余时间. 单位为 tick, 1024 tick 为 1 秒.
uint64_t buffTimeLeftTick(int buffID, int buffLevel);

// 施加 buff.
// buffSourceID 为施加者 ID, buffSourceLevel 为施加者等级.
// count 为持续时间倍数, 默认为 1.
// stacknum 为 buff 层数, 默认为 1.
// 这个函数可以使用 4~7 个参数的版本. 如果参数数量小于 7, 则缺少的参数会使用默认值.
void buffAdd(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int count, int param6, int stacknum);

void buffDel(int buffID, int buffLevel);      // 移除一层指定的 buff
void buffDelGroup(int buffID, int buffLevel); // 移除所有层数指定的 buff
void buffDelMultiGroupByID(int buffID);       // 移除指定 ID 的所有 buff

// 设置 buff 剩余跳数.
// buffIndex 为 buff 实例的索引, 应当从 buffGet 的返回值中获取.
void buffSetLeftActiveCount(int buffIndex, int count);

// 设置 buff 距离下一跳的帧数.
// buffIndex 为 buff 实例的索引, 应当从 buffGet 的返回值中获取.
// nextActiveFrame 为距离下一跳的帧数.
// 注意, 16 帧为 1 秒. (此处使用的单位不是 tick.)
void buffSetNextActiveFrame(int buffIndex, int nextActiveFrame);

void cooldownClearTime(int cooldownID); // 清除指定 cooldownID 的 CD

// 修改指定 cooldownID 的 CD.
// frame 为修改的帧数, 正数代表增加 CD, 负数代表减少 CD.
// 注意, 16 帧为 1 秒. (此处使用的单位不是 tick.)
void cooldownModify(int cooldownID, int frame);

void cooldownReset(int cooldownID); // 重置指定 cooldownID 的 CD

// 获取指定技能的等级.
// 如果技能尚未学习, 则返回 0.
int skillGetLevel(int skillID);

// 以下三个函数为重载函数, 使用任意版本均可, 程序会根据参数数量自动确定调用版本.
// 不推荐使用此方式释放技能. 更为推荐的方式是, skillLearn 后使用 cast 的版本.
void skillCast(int skillID, int skillLevel);
void skillCast(int skillID, int skillLevel, int targetID);
void skillCast(int skillID, int skillLevel, int type, int targetID);

// 向指定坐标施放指定 ID 和 Level 的技能.
// 该函数应当废弃使用.
// 坐标系统未真正使用, 请使用 (0, 0, 0) 代替, 或不使用该函数.
void skillCastXYZ(int skillID, int skillLevel, int x, int y, int z);

bool skillrecipeExist(int RecipeID, int RecipeLevel); // 判断是否存在指定的秘籍

// 以下函数请自行查询 JX3Calc 源码.
// 如果你想要设置计时器 (例如, 某特定时间段后指定某操作),
// 那么你需要使用的是 delayCustom 属性, 而不是这些函数.
void timerSet(int frame, string filename, int targetID);
void timerSet(int frame, string filename, int type, int targetID);

// 以下函数请自行查询 JX3Calc 源码.
bool attrImportFromData(string dataJsonStr); // 从 JSON 字符串导入角色属性
bool attrImportFromJX3BOX(string pzID);      // 从 JX3BOX 导入角色属性

// 以下函数为低级封装接口, 专为兼容游戏内宏而设, 不推荐使用.

// 释放技能.
// 参数为技能名称. 返回技能释放成功与否.
bool macroSkillCast(string skillName);

// 查询 buff 层数. 对应游戏内宏 buff 条件语句.
// 如果没有 buff, 返回 0.
// 注意! 0 在 lua 中直接作为 if 的 condition 时, 会被转换为 true!
int macroBuff(string buffName);

// 查询是否没有 buff. 对应游戏内宏 nobuff 条件语句.
// 没有则返回 true.
bool macroNoBuff(string buffName);

// 查询 buff 剩余时间. 对应游戏内宏 bufftime 条件语句.
// 如果没有 buff, 返回 0.
// 注意! 0 在 lua 中直接作为 if 的 condition 时, 会被转换为 true!
double macroBufftime(string buffName);

int    macroTBuff(string buffName);     // macroBuff 的 target 版本
bool   macroTNoBuff(string buffName);   // macroNoBuff 的 target 版本
double macroTBufftime(string buffName); // macroBufftime 的 target 版本

// --------------------分割线--------------------
// ChAttr. 它只能通过 player.chAttr 进行调用.

int atVitalityBase;                   // 体质
int atStrengthBase;                   // 力道
int atAgilityBase;                    // 身法
int atSpiritBase;                     // 根骨
int atSpunkBase;                      // 元气
int atPhysicsAttackPowerBase;         // 外功基础攻击
int atMagicAttackPowerBase;           // 内功基础攻击
int atSolarAttackPowerBase;           // 阳性内功基础攻击
int atLunarAttackPowerBase;           // 阴性内功基础攻击
int atNeutralAttackPowerBase;         // 混元内功基础攻击
int atPoisonAttackPowerBase;          // 毒性内功基础攻击
int atTherapyPowerBase;               // 治疗量
int atPhysicsCriticalStrike;          // 外功会心等级
int atSolarCriticalStrike;            // 阳性内功会心等级
int atLunarCriticalStrike;            // 阴性内功会心等级
int atNeutralCriticalStrike;          // 混元内功会心等级
int atPoisonCriticalStrike;           // 毒性内功会心等级
int atPhysicsCriticalDamagePowerBase; // 外功会心效果等级
int atSolarCriticalDamagePowerBase;   // 阳性内功会心效果等级
int atLunarCriticalDamagePowerBase;   // 阴性内功会心效果等级
int atNeutralCriticalDamagePowerBase; // 混元内功会心效果等级
int atPoisonCriticalDamagePowerBase;  // 毒性内功会心效果等级
int atPhysicsOvercomeBase;            // 外功基础破防等级
int atSolarOvercomeBase;              // 阳性内功破防等级
int atLunarOvercomeBase;              // 阴性内功破防等级
int atNeutralOvercomeBase;            // 混元内功破防等级
int atPoisonOvercomeBase;             // 毒性内功破防等级
int atSurplusValueBase;               // 破招值
int atStrainBase;                     // 无双等级
int atHasteBase;                      // 基础加速等级
int atPhysicsShieldBase;              // 外功基础防御等级
int atSolarMagicShieldBase;           // 阳性内功防御等级
int atLunarMagicShieldBase;           // 阴性内功防御等级
int atNeutralMagicShieldBase;         // 混元内功防御等级
int atPoisonMagicShieldBase;          // 毒性内功防御等级
int atDodge;                          // 闪避等级
int atParryBase;                      // 招架等级
int atParryValueBase;                 // 拆招值
int atToughnessBase;                  // 御劲等级
int atDecriticalDamagePowerBase;      // 化劲等级
int atMeleeWeaponDamageBase;          // 武器伤害
int atMeleeWeaponDamageRand;          // 武器伤害浮动

// --------------------分割线--------------------
// BuffItem. 它只能通过 player.buffGet 的返回值进行调用.

int nLevel;           // buff 等级
int nIndex;           // buff 索引, 用于 buffSetLeftActiveCount 和 buffSetNextActiveFrame
int nStackNum;        // buff 层数
int nLeftActiveCount; // buff 剩余跳数
int nNextActiveFrame; // buff 距离下一跳的帧数
int nCustomValue;     // 自定义值
