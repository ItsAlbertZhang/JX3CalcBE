#ifndef FRAME_CHARACTER_H_
#define FRAME_CHARACTER_H_

#include "frame/character/property/attribute.h"
#include "frame/character/property/buff.h"
#include "frame/character/property/cooldown.h"
#include "frame/character/property/damage.h"
#include "frame/character/property/scene.h"
#include "frame/character/property/skill.h"
#include "frame/character/property/skillevent.h"
#include "frame/character/property/skillrecipe.h"
#include "frame/ref/lua_other.h"
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace ns_frame {

using CharacterType = ref::enumLuaTarget;

/**
 * @brief Character 类
 * @note 用于表示游戏中的角色.
 * @warning #### 关于线程安全:
 * @warning 在构造实例时, 使用 thread_local 确保线程安全.
 */
class Character {
public:
    Character(); // 构造函数, 线程安全通过 thread_local 保证

    bool isPlayer = false;             // 是否为玩家
    Character *targetSelect = nullptr; // 选中的目标
    Character *targetCurr = nullptr;   // 当前目标
    ref::enumLuaSkillKindType atAdaptiveSkillType = ref::enumLuaSkillKindType::COUNT;

    CharacterAttr chAttr;               // 角色属性. 这一属性暂时被 api.lua 调用.
    CharacterBuff chBuff;               // 角色 buff
    CharacterCooldown chCooldown;       // 角色冷却
    CharacterDamage chDamage;           // 角色伤害
    CharacterSkill chSkill;             // 角色技能
    CharacterSkillRecipe chSkillRecipe; // 角色技能秘籍
    CharacterSkillEvent chSkillEvent;   // 角色技能事件
    CharacterScene chScene;             // 角色场景

    // ---------- 以下方法未被 lua 调用 ----------

    // character
    static Character *characterGet(int nCharacterID);
    static int characterGetID(Character *character);
    // buff
    void buffBind(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int skillID, int skillLevel);
    void buffFlushLeftFrame(CharacterBuff::Item *item);
    CharacterBuff::Item *buffGetWithCompareFlag(int buffID, int buffLevel, int flag);
    CharacterBuff::Item *buffGetByOwnerWithCompareFlag(int buffID, int buffLevel, int sourceID, int flag);
    // calc
    std::tuple<int, int> calcCritical(const CharacterAttr &attrSelf, int skillID, int skillLevel);
    int calcDamage(const CharacterAttr &attrSelf, Character *target, DamageType typeDamage, bool isCritical, int atCriticalDamagePower, int DamageAddPercent, int damageBase, int damageRand, int nChannelInterval, int nWeaponDamagePercent, int dotInterval = 1, int dotCount = 1, bool isSurplus = false);
    // skill
    bool skillCast(Character *target, int skillID, int skillLevel);
    void skillDeactive(int skillID);
    // skillrecipe
    void skillrecipeRemove(int recipeID, int recipeLevel);
    std::set<const SkillRecipe *> skillrecipeGet(int skillID, int skillrecipeType);
    // skillevent
    void skilleventAdd(int eventID);
    void skilleventRemove(int eventID);
    std::set<const SkillEvent *> skilleventGet(ref::enumSkilleventEventtype type, int eventskillID, uint32_t eventmask1, uint32_t eventmask2);

    // ---------- 以下属性和方法暂时被 api.lua 调用. 这些函数在 lua 内的名称和此处是相同的. ----------
    void skillLearn(int skillID, int skillLevel);
    void skillActive(int skillID);
    void cast(int skillID);
    void skillrecipeAdd(int recipeID, int recipeLevel);
    void vCheckSunMoonPower();
    int dwKungfuID = 0;
    int publicCooldownID = 0;
    int macroNum = 0;
    int macroIdx = 0;
    int delayBase = 0;
    int delayRand = 0;

    // ---------- 以下方法直接被 lua 调用. 注意, 这些函数在 lua 内的名称是不同的, 详情可查 frame/lua_static.cpp ----------

    // character
    Character *characterGetSelect();
    int characterGetTargetID();
    // cooldown
    void cooldownClearTime(int cooldownID);
    void cooldownModify(int cooldownID, int frame);
    void cooldownReset(int cooldownID);
    // buff
    bool buffExist(int buffID, int buffLevel);
    CharacterBuff::Item *buffGet(int buffID, int buffLevel);
    CharacterBuff::Item *buffGetByOwner(int buffID, int buffLevel, int sourceID);
    void buffAdd4(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel);
    void buffAdd5(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int count);
    void buffAdd7(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int count, int param6, int stacknum);
    void buffDel(int buffID, int buffLevel);
    void buffDelGroup(int buffID, int buffLevel);
    void buffDelMultiGroupByID(int buffID);
    void buffSetLeftActiveCount(int buffIndex, int count);
    void buffSetNextActiveFrame(int buffIndex, int nextActiveFrame);
    // skill
    int skillGetLevel(int skillID);
    void skillCast2(int skillID, int skillLevel);
    void skillCast3(int skillID, int skillLevel, int targetID);
    void skillCast4(int skillID, int skillLevel, int type, int targetID);
    void skillCastXYZ(int skillID, int skillLevel, int x, int y, int z);
    // skillrecipe
    bool skillrecipeExist(int RecipeID, int RecipeLevel);
    // scene
    CharacterScene *sceneGet();
    // timer
    void timerSet3(int frame, std::string filename, int targetID);
    void timerSet4(int frame, std::string filename, int type, int targetID);
    // other
    bool otherIsFormationLeader();
    bool otherIsInParty();
    int otherGetKungfuMountID();
    int otherGetMapID();
    void otherClearAllNpcThreat();
    void otherCreatePublicShadow(int a, int roletype, int dwID, int nX, int nY, int nZ, bool b);
    void otherDestroyPublicShadow(int a, bool b);
    void otherDoAction(int a, int b);
    void otherPlayPublicShadowAnimation(int a, int b, bool c, bool d);

    //  ---------- 被 lua 调用的属性, 通常为匈牙利命名法 ----------
    int dwID;                          // 角色 ID
    int nLevel = chAttr.atLevel;       // 等级, 这是唯一一个同时存在于此处和 chAttr 内部的属性
    int nX, nY, nZ;                    // 坐标
    int nCurrentSunEnergy = 0;         // 当前日灵
    int nCurrentMoonEnergy = 0;        // 当前月魂
    int nSunPowerValue = 0;            // 满日
    int nMoonPowerValue = 0;           // 满月
    int nRoleType = 0;                 // 角色体型
    bool bSurplusAutoCast = false;     // 出现于 明教_套路_内功_焚影圣诀.lua
    bool bSurplusAutoReplenish = true; // 出现于 明教_套路_内功_焚影圣诀.lua
    bool bFightState = false;          // 是否处于战斗状态
    double fMaxLife64 = 0;             // 最大生命值
    double fCurrentLife64 = 0;         // 当前生命值

private:
    static inline thread_local std::vector<Character *> characterList;            // 角色列表
    static inline thread_local std::unordered_map<Character *, int> characterMap; // 角色表
};

class Player : public Character {
};

class NPC : public Character {
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_H_