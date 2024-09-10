#pragma once

#include "frame/character/property/attribute.h"
#include "frame/character/property/buff.h"
#include "frame/character/property/cooldown.h"
#include "frame/character/property/item.h"
#include "frame/character/property/scene.h"
#include "frame/character/property/skill.h"
#include "frame/character/property/skillevent.h"
#include "frame/character/property/skillrecipe.h"
#include "frame/common/damage.h"
#include "frame/ref/lua_normal.h"
#include "frame/ref/ref.h"
#include <cstdint>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace jx3calc {
namespace frame {

class AutoRollbackAttrib;
class AutoRollbackAttribute;

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

    ref::lua::SKILL_KIND_TYPE atAdaptiveSkillType;

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
    int mountID     = 0; // 门派 ID, 见 skills.tab 中的 MountRequestType
    int kungfuID    = 0; // 心法 ID, 是一个技能 ID, 游戏内按住 Ctrl 并将鼠标放在心法图标上即可看到
    int kungfuLevel = 0;

    // character
    static auto characterGet(int nCharacterID) -> Character *;
    static int  characterGetID(Character *character);

    // attr
    bool attrImportFromData(std::string dataJsonStr);
    bool attrImportFromJX3BOX(std::string pzID);
    bool attrImportFromBackup(const ChAttr &attr);
    // buff
    auto buffTimeLeftTick(int buffID, int buffLevel) -> event_tick_t;
    auto buffGetWithCompareFlag(int buffID, int buffLevel, int flag) -> BuffItem *;
    auto buffGetByOwnerWithCompareFlag(int buffID, int buffLevel, int sourceID, int flag) -> BuffItem *;
    void buffBind(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int skillID, int skillLevel);
    void buffFlushLeftFrame(BuffItem *item);
    // cooldown
    void cooldownModify(int cooldownID, int frame, int count);
    auto cooldownLeft(int cooldownID) -> event_tick_t;
    // item
    void itemAdd(ItemType type, int ID);
    void itemAdd(int type, int ID);
    void itemUse(ItemType type, int ID);
    void itemUse(int type, int ID);
    // skill
    auto skillCooldownLeftTick(int skillID) -> event_tick_t;
    bool cast(int skillID);
    bool skillCast(Character *target, int skillID, int skillLevel);
    int  skillCountAvailable(int skillID);
    void skillActive(int skillID);
    void skillDeactive(int skillID);
    // skillrecipe
    auto skillrecipeGet(int skillID, int skillrecipeType) -> std::set<const SkillRecipe *>;
    void skillrecipeAdd(int recipeID, int recipeLevel);
    void skillrecipeRemove(int recipeID, int recipeLevel);
    // skillevent
    auto skilleventGet(ref::SkillEvent::EventType type, int eventskillID, uint32_t eventmask1, uint32_t eventmask2) -> std::set<const SkillEvent *>;
    void skilleventAdd(int eventID);
    void skilleventRemove(int eventID);
    bool skilleventExist(int eventID);

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
        int              buffCount,
        bool             isFrost
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

    // ---------- 以下方法直接被游戏 lua 调用. 注意, 这些函数在 lua 内的名称是不同的, 详情可查 frame/lua/static.cpp ----------

    // character
    auto characterGetEmployer() -> Character *;
    auto characterGetSelect() -> Character *;
    int  characterGetTargetID();
    // buff
    auto buffGet(int buffID, int buffLevel) -> BuffItem *;
    auto buffGetByOwner(int buffID, int buffLevel, int sourceID) -> BuffItem *;
    bool buffExist(int buffID, int buffLevel);
    void buffAdd(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int count = 1, int param6 = 0, int stacknum = 1);
    void buffAddOptional(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, std::optional<int> count, std::optional<int> param6, std::optional<int> stacknum);
    void buffDel(int buffID, int buffLevel);
    void buffDelMultiCount(int buffID, int buffLevel, int count);
    void buffDelGroup(int buffID, int buffLevel);
    void buffDelMultiGroupByID(int buffID);
    void buffSetLeftActiveCount(int buffIndex, int count);
    void buffSetNextActiveFrame(int buffIndex, int nextActiveFrame);
    // cooldown
    void cooldownClearTime(int cooldownID);
    void cooldownModify(int cooldownID, int frame);
    void cooldownReset(int cooldownID);
    // skill
    int  skillGetLevel(int skillID);
    void skillLearn(int skillID, int skillLevel);
    void skillCast(int skillID, int skillLevel);
    void skillCast(int skillID, int skillLevel, int targetID);
    void skillCast(int skillID, int skillLevel, int type, int targetID);
    void skillCastXYZ(int skillID, int skillLevel, int x, int y, int z);
    // skillrecipe
    bool skillrecipeExist(int RecipeID, int RecipeLevel);
    // scene
    auto sceneGet() -> ChScene *;
    // timer
    void timerSet(int frame, std::string filename, int targetID);
    void timerSet(int frame, std::string filename, int type, int targetID);
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
    void otherSuperCustomDamage(int sourceID, int skillID, int skillLevel, int type, int damage);

    //  ---------- 被游戏 lua 调用的属性 ----------
    int    dwID;                          // 角色 ID
    int    nX, nY, nZ;                    // 坐标
    int    nCurrentSunEnergy     = 0;     // 当前日灵
    int    nCurrentMoonEnergy    = 0;     // 当前月魂
    int    nSunPowerValue        = 0;     // 满日
    int    nMoonPowerValue       = 0;     // 满月
    int    nRoleType             = 0;     // 角色体型
    int    nTouchRange           = 0;     // 距离补偿
    bool   bSurplusAutoCast      = false; // 破招自动施展
    bool   bSurplusAutoReplenish = true;  // 破招自动充能
    bool   bFightState           = false; // 战斗状态
    double fMaxLife64            = 0;     // 最大气血值
    double fCurrentLife64        = 0;     // 当前气血值

    int nLevel_getter() {
        return chAttr.atLevel;
    }
    void nLevel_setter(int value) {
        chAttr.atLevel = value;
    }

private:
    static inline thread_local std::vector<Character *>             characterList; // 角色列表
    static inline thread_local std::unordered_map<Character *, int> characterMap;  // 角色表
};

} // namespace frame
} // namespace jx3calc
