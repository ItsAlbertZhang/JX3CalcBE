#ifndef FRAME_CHARACTER_H_
#define FRAME_CHARACTER_H_

#include "frame/character/property/attribute.h"
#include "frame/character/property/buff.h"
#include "frame/character/property/cooldown.h"
#include "frame/character/property/damage.h"
#include "frame/character/property/item.h"
#include "frame/character/property/scene.h"
#include "frame/character/property/skill.h"
#include "frame/character/property/skillevent.h"
#include "frame/character/property/skillrecipe.h"
#include "frame/ref/lua_other.h"
#include <cstdint>
#include <optional>
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

    Character(const Character &) = delete;
    Character(Character &&)      = delete;

    bool       isPlayer     = false;   // 是否为玩家
    Character *targetSelect = nullptr; // 选中的目标
    Character *targetCurr   = nullptr; // 当前目标

    ref::enumLuaSkillKindType atAdaptiveSkillType = ref::enumLuaSkillKindType::COUNT;

    ChAttr        chAttr;        // 角色属性. 这一属性暂时被 api.lua 调用.
    ChBuff        chBuff;        // 角色 buff
    ChCooldown    chCooldown;    // 角色冷却
    ChDamage      chDamage;      // 角色伤害
    ChItem        chItem;        // 角色物品
    ChSkill       chSkill;       // 角色技能
    ChSkillRecipe chSkillRecipe; // 角色技能秘籍
    ChSkillEvent  chSkillEvent;  // 角色技能事件
    ChScene       chScene;       // 角色场景

    std::set<AutoRollbackAttrib *>    autoRollbackAttribList;    // 自动回滚的 buff 属性列表
    std::set<AutoRollbackAttribute *> autoRollbackAttributeList; // 自动回滚的魔法属性列表

    // ---------- 以下属性和方法未被游戏 lua 调用 ----------
    int                           dwKungfuID = 0;
    // character
    static int                    characterGetID(Character *character);
    static Character             *characterGet(int nCharacterID);
    // attr
    bool                          attrImportFromData(std::string dataJsonStr);
    bool                          attrImportFromJX3BOX(std::string pzID);
    bool                          attrImportFromBackup(const ChAttr &attr);
    // buff
    void                          buffBind(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int skillID, int skillLevel);
    void                          buffFlushLeftFrame(BuffItem *item);
    event_tick_t                  buffTimeLeftTick(int buffID, int buffLevel);
    BuffItem                     *buffGetWithCompareFlag(int buffID, int buffLevel, int flag);
    BuffItem                     *buffGetByOwnerWithCompareFlag(int buffID, int buffLevel, int sourceID, int flag);
    // item
    void                          itemAdd(ItemType type, int ID);
    void                          itemAdd(int type, int ID);
    void                          itemUse(ItemType type, int ID);
    void                          itemUse(int type, int ID);
    // skill
    bool                          cast(int skillID);
    bool                          skillCast(Character *target, int skillID, int skillLevel);
    event_tick_t                  skillCooldownLeftTick(int skillID);
    int                           skillCountAvailable(int skillID);
    void                          skillActive(int skillID);
    void                          skillDeactive(int skillID);
    void                          skillLearn(int skillID, int skillLevel);
    // skillrecipe
    void                          skillrecipeAdd(int recipeID, int recipeLevel);
    void                          skillrecipeRemove(int recipeID, int recipeLevel);
    std::set<const SkillRecipe *> skillrecipeGet(int skillID, int skillrecipeType);
    // skillevent
    void                          skilleventAdd(int eventID);
    void                          skilleventRemove(int eventID);
    std::set<const SkillEvent *>  skilleventGet(ref::enumSkilleventEventtype type, int eventskillID, uint32_t eventmask1, uint32_t eventmask2);

    // calc
    Damage calcDamage(
        int              id,
        int              level,
        const ChAttr    &attrSelf,
        const Character *target,
        DamageType       typeDamage,
        int              atCriticalStrike,
        int              atCriticalDamagePower,
        int              damageBase,
        int              damageRand,
        int              damageAddPercent,
        int              nChannelInterval,
        int              nWeaponDamagePercent,
        bool             isCritical,
        bool             isSurplus,
        bool             isBuff,
        int              buffInterval,
        int              buffCount
    );
    std::tuple<int, int> calcCritical(const ChAttr &attrSelf, int skillID, int skillLevel);

    // macro parse
    bool   macroSkillCast(std::string skillName);
    int    macroBuff(std::string buffName);
    bool   macroNoBuff(std::string buffName);
    double macroBufftime(std::string buffName);
    int    macroTBuff(std::string buffName);
    bool   macroTNoBuff(std::string buffName);
    double macroTBufftime(std::string buffName);

    // ---------- 以下方法直接被游戏 lua 调用. 注意, 这些函数在 lua 内的名称是不同的, 详情可查 frame/lua_static.cpp ----------

    // character
    int        characterGetTargetID();
    Character *characterGetSelect();
    // buff
    bool       buffExist(int buffID, int buffLevel);
    void       buffAdd(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int count = 1, int param6 = 0, int stacknum = 1);
    void       buffAddOptional(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, std::optional<int> count, std::optional<int> param6, std::optional<int> stacknum);
    void       buffDel(int buffID, int buffLevel);
    void       buffDelGroup(int buffID, int buffLevel);
    void       buffDelMultiGroupByID(int buffID);
    void       buffSetLeftActiveCount(int buffIndex, int count);
    void       buffSetNextActiveFrame(int buffIndex, int nextActiveFrame);
    BuffItem  *buffGet(int buffID, int buffLevel);
    BuffItem  *buffGetByOwner(int buffID, int buffLevel, int sourceID);
    // cooldown
    void       cooldownClearTime(int cooldownID);
    void       cooldownModify(int cooldownID, int frame);
    void       cooldownReset(int cooldownID);
    // skill
    int        skillGetLevel(int skillID);
    void       skillCast(int skillID, int skillLevel);
    void       skillCast(int skillID, int skillLevel, int targetID);
    void       skillCast(int skillID, int skillLevel, int type, int targetID);
    void       skillCastXYZ(int skillID, int skillLevel, int x, int y, int z);
    // skillrecipe
    bool       skillrecipeExist(int RecipeID, int RecipeLevel);
    // scene
    ChScene   *sceneGet();
    // timer
    void       timerSet(int frame, std::string filename, int targetID);
    void       timerSet(int frame, std::string filename, int type, int targetID);
    // other
    bool       otherIsFormationLeader();
    bool       otherIsInParty();
    int        otherGetKungfuMountID();
    int        otherGetMapID();
    void       otherClearAllNpcThreat();
    void       otherCreatePublicShadow(int a, int roletype, int dwID, int nX, int nY, int nZ, bool b);
    void       otherDestroyPublicShadow(int a, bool b);
    void       otherDoAction(int a, int b);
    void       otherPlayPublicShadowAnimation(int a, int b, bool c, bool d);

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
