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
#include <cstdint>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace ns_frame {

class AutoRollbackAttrib;
class AutoRollbackAttribute;

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
    virtual ~Character();

    bool       isPlayer     = false;   // 是否为玩家
    Character *targetSelect = nullptr; // 选中的目标
    Character *targetCurr   = nullptr; // 当前目标

    ref::enumLuaSkillKindType atAdaptiveSkillType = ref::enumLuaSkillKindType::COUNT;

    ChAttr        chAttr;        // 角色属性. 这一属性暂时被 api.lua 调用.
    ChBuff        chBuff;        // 角色 buff
    ChCooldown    chCooldown;    // 角色冷却
    ChDamage      chDamage;      // 角色伤害
    ChSkill       chSkill;       // 角色技能
    ChSkillRecipe chSkillRecipe; // 角色技能秘籍
    ChSkillEvent  chSkillEvent;  // 角色技能事件
    ChScene       chScene;       // 角色场景

    std::set<AutoRollbackAttrib *>    autoRollbackAttribList;    // 自动回滚的 buff 属性列表
    std::set<AutoRollbackAttribute *> autoRollbackAttributeList; // 自动回滚的魔法属性列表

    // ---------- 以下属性和方法未被游戏 lua 调用 ----------
    int dwKungfuID = 0;
    // character
    static int        characterGetID(Character *character);
    static Character *characterGet(int nCharacterID);
    // attr
    void   attrImportFromJX3BOX(int pzID);
    void   attrImportFromBackup(const ChAttr &attr);
    ChAttr attrExport();
    // buff
    void      buffBind(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int skillID, int skillLevel);
    void      buffFlushLeftFrame(BuffItem *item);
    BuffItem *buffGetWithCompareFlag(int buffID, int buffLevel, int flag);
    BuffItem *buffGetByOwnerWithCompareFlag(int buffID, int buffLevel, int sourceID, int flag);
    // calc
    int calcDamage(
        const ChAttr &attrSelf,
        Character    *target,
        DamageType    typeDamage,
        bool          isCritical,
        int           atCriticalDamagePower,
        int           DamageAddPercent,
        int           damageBase,
        int           damageRand,
        int           nChannelInterval,
        int           nWeaponDamagePercent,
        int           dotInterval = 1,
        int           dotCount    = 1,
        bool          isSurplus   = false
    );
    std::tuple<int, int> calcCritical(const ChAttr &attrSelf, int skillID, int skillLevel);
    // skill
    void cast(int skillID);
    bool skillCast(Character *target, int skillID, int skillLevel);
    void skillActive(int skillID);
    void skillDeactive(int skillID);
    void skillLearn(int skillID, int skillLevel);
    // skillrecipe
    void                          skillrecipeAdd(int recipeID, int recipeLevel);
    void                          skillrecipeRemove(int recipeID, int recipeLevel);
    std::set<const SkillRecipe *> skillrecipeGet(int skillID, int skillrecipeType);
    // skillevent
    void                         skilleventAdd(int eventID);
    void                         skilleventRemove(int eventID);
    std::set<const SkillEvent *> skilleventGet(ref::enumSkilleventEventtype type, int eventskillID, uint32_t eventmask1, uint32_t eventmask2);

    // ---------- 以下方法直接被游戏 lua 调用. 注意, 这些函数在 lua 内的名称是不同的, 详情可查 frame/lua_static.cpp ----------

    // character
    int        characterGetTargetID();
    Character *characterGetSelect();
    // buff
    bool      buffExist(int buffID, int buffLevel);
    void      buffAdd4(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel);
    void      buffAdd5(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int count);
    void      buffAdd7(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int count, int param6, int stacknum);
    void      buffDel(int buffID, int buffLevel);
    void      buffDelGroup(int buffID, int buffLevel);
    void      buffDelMultiGroupByID(int buffID);
    void      buffSetLeftActiveCount(int buffIndex, int count);
    void      buffSetNextActiveFrame(int buffIndex, int nextActiveFrame);
    BuffItem *buffGet(int buffID, int buffLevel);
    BuffItem *buffGetByOwner(int buffID, int buffLevel, int sourceID);
    // cooldown
    void cooldownClearTime(int cooldownID);
    void cooldownModify(int cooldownID, int frame);
    void cooldownReset(int cooldownID);
    // skill
    int  skillGetLevel(int skillID);
    void skillCast2(int skillID, int skillLevel);
    void skillCast3(int skillID, int skillLevel, int targetID);
    void skillCast4(int skillID, int skillLevel, int type, int targetID);
    void skillCastXYZ(int skillID, int skillLevel, int x, int y, int z);
    // skillrecipe
    bool skillrecipeExist(int RecipeID, int RecipeLevel);
    // scene
    ChScene *sceneGet();
    // timer
    void timerSet3(int frame, std::string filename, int targetID);
    void timerSet4(int frame, std::string filename, int type, int targetID);
    // other
    bool otherIsFormationLeader();
    bool otherIsInParty();
    int  otherGetKungfuMountID();
    int  otherGetMapID();
    void otherClearAllNpcThreat();
    void otherCreatePublicShadow(int a, int roletype, int dwID, int nX, int nY, int nZ, bool b);
    void otherDestroyPublicShadow(int a, bool b);
    void otherDoAction(int a, int b);
    void otherPlayPublicShadowAnimation(int a, int b, bool c, bool d);

    //  ---------- 被游戏 lua 调用的属性, 通常为匈牙利命名法 ----------
    int    dwID;                          // 角色 ID
    int    nLevel = chAttr.atLevel;       // 等级, 这是唯一一个同时存在于此处和 chAttr 内部的属性
    int    nX, nY, nZ;                    // 坐标
    int    nCurrentSunEnergy     = 0;     // 当前日灵
    int    nCurrentMoonEnergy    = 0;     // 当前月魂
    int    nSunPowerValue        = 0;     // 满日
    int    nMoonPowerValue       = 0;     // 满月
    int    nRoleType             = 0;     // 角色体型
    int    nTouchRange           = 0;     // 距离补偿
    bool   bSurplusAutoCast      = false; // 出现于 明教_套路_内功_焚影圣诀.lua
    bool   bSurplusAutoReplenish = true;  // 出现于 明教_套路_内功_焚影圣诀.lua
    bool   bFightState           = false; // 是否处于战斗状态
    double fMaxLife64            = 0;     // 最大生命值
    double fCurrentLife64        = 0;     // 当前生命值

private:
    static inline thread_local std::vector<Character *>             characterList; // 角色列表
    static inline thread_local std::unordered_map<Character *, int> characterMap;  // 角色表
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_H_
