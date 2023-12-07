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
#include "frame/static_ref.h"
#include <mutex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace ns_frame {

using CharacterType = ns_framestatic::enumLuaTarget;

/**
 * @brief Character 类
 * @note 用于表示游戏中的角色.
 * @warning #### 关于线程安全:
 * @warning 在构造实例时, 使用线程锁确保线程安全.
 */
class Character {
public:
    Character(); // 构造函数

    bool isPlayer = false;             // 是否为玩家
    Character *targetSelect = nullptr; // 选中的目标
    Character *targetCurr = nullptr;   // 当前目标
    ns_framestatic::enumLuaSkillKindType atAdaptiveSkillType = ns_framestatic::enumLuaSkillKindType::COUNT;
    int dwKungfuID = 0;

    CharacterAttr chAttr;               // 角色属性. 这一属性暂时被 api.lua 调用.
    CharacterBuff chBuff;               // 角色 buff
    CharacterCooldown chCooldown;       // 角色冷却
    CharacterDamage chDamage;           // 角色伤害
    CharacterSkill chSkill;             // 角色技能
    CharacterSkillRecipe chSkillRecipe; // 角色技能秘籍
    CharacterSkillEvent chSkillEvent;   // 角色技能事件

    // ---------- 以下方法未被 lua 调用 ----------
    static Character *getCharacter(int nCharacterID);
    static int getCharacterID(Character *character);
    void DeactiveSkill(int skillID);
    bool CastSkill(Character *target, int skillID, int skillLevel);
    void DelBuffAllStackNum(CharacterBuff::Item &it);
    void BindBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int skillID, int skillLevel);
    CharacterBuff::Item *GetBuffWithCompareFlag(int buffID, int buffLevel, int flag);
    CharacterBuff::Item *GetBuffByOwnerWithCompareFlag(int buffID, int buffLevel, int sourceID, int flag);
    std::tuple<int, int> CalcCritical(const CharacterAttr &attrSelf, int skillID, int skillLevel);
    int CalcDamage(const CharacterAttr &attrSelf, Character *target, DamageType typeDamage, bool isCritical, int atCriticalDamagePower, int DamageAddPercent, int damageBase, int damageRand, int nChannelInterval, int nWeaponDamagePercent, int dotInterval = 1, int dotCount = 1, bool isSurplus = false);

    // ---------- 以下属性和方法暂时被 api.lua 调用 ----------
    void SetSkillRecipe(int recipeID, int recipeLevel);
    void LearnSkill(int skillID, int skillLevel);
    void ActiveSkill(int skillID);
    void Cast(int skillID);
    void CheckSunMoonPower();
    int publicCooldownID = 0;
    int macroNum = 0;
    int macroIdx = 0;
    int delayBase = 0;
    int delayRand = 0;

    // ---------- 以下方法直接被 lua 调用 ----------
    bool IsFormationLeader();
    bool IsHaveBuff(int buffID, int buffLevel);
    bool IsInParty();
    bool IsSkillRecipeActive(int RecipeID, int RecipeLevel);
    CharacterBuff::Item *GetBuff(int buffID, int buffLevel);
    CharacterBuff::Item *GetBuffByOwner(int buffID, int buffLevel, int sourceID);
    const CharacterScene &GetScene();
    int GetKungfuMountID();
    int GetSkillLevel(int skillID);
    int GetSkillTarget();
    Character *GetSelectCharacter();
    int GetMapID();
    void AddBuff4(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel);
    void AddBuff5(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int count);
    void AddBuff7(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int count, int param6, int stacknum);
    void DelBuff(int buffID, int buffLevel);
    void CastSkill2(int skillID, int skillLevel);
    void CastSkill3(int skillID, int skillLevel, int targetID);
    void CastSkill4(int skillID, int skillLevel, int type, int targetID);
    void CastSkillXYZ(int skillID, int skillLevel, int x, int y, int z);
    void ModifyCoolDown(int cooldownID, int frame);
    void SetTimer3(int frame, std::string filename, int targetID);
    void SetTimer4(int frame, std::string filename, int type, int targetID);
    void PlayPublicShadowAnimation(int a, int b, bool c, bool d);
    void ClearAllNpcThreat();
    void ClearCDTime(int cooldownID);
    void DelGroupBuff(int buffID, int buffLevel);
    void DelMultiGroupBuffByID(int buffID);
    void DoAction(int a, int b);
    void ResetCD(int cooldownID);
    void DestroyPublicShadow(int a, bool b);
    void CreatePublicShadow(int a, int roletype, int dwID, int nX, int nY, int nZ, bool b);
    void SetBuffLeftActiveCount(int buffIndex, int count);
    void SetBuffNextActiveFrame(int buffIndex, int nextActiveFrame);

    //  ---------- 被 lua 调用的属性, 通常以类型标志开头 ----------
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
    static inline std::mutex mutex;                                  // 互斥锁. 用于保护构造操作.
    static inline std::vector<Character *> characterList;            // 角色列表
    static inline std::unordered_map<Character *, int> characterMap; // 角色表
};

class Player : public Character {
};

class NPC : public Character {
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_H_