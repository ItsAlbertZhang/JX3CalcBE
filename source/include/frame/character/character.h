#ifndef FRAME_CHARACTER_H_
#define FRAME_CHARACTER_H_

#include "frame/character/property/attribute.h"
#include "frame/character/property/buff.h"
#include "frame/character/property/cooldown.h"
#include "frame/character/property/damage.h"
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
/**
 * @brief Character 类
 * @note 用于表示游戏中的角色.
 * @warning #### 关于线程安全:
 * @warning 在构造实例时, 使用线程锁确保线程安全.
 */
class Character {
public:
    Character(); // 构造函数

    bool isPlayer = false;    // 是否为玩家
    Character *target = this; // 当前目标
    bool isOutOfFight = true; // 是否处于战斗状态
    ns_framestatic::enumLuaSkillKindType atAdaptiveSkillType = ns_framestatic::enumLuaSkillKindType::COUNT;
    int dwKungfuID = 0;

    CharacterAttr chAttr;               // 角色属性
    CharacterBuff chBuff;               // 角色 buff
    CharacterCooldown chCooldown;       // 角色冷却
    CharacterDamage chDamage;           // 角色伤害
    CharacterSkill chSkill;             // 角色技能
    CharacterSkillRecipe chSkillRecipe; // 角色技能秘籍
    CharacterSkillEvent chSkillEvent;   // 角色技能事件

    // ---------- 以下方法未被 lua 调用 ----------
    static Character *getCharacter(int nCharacterID);
    static int getCharacterID(Character *character);
    void LearnSkill(int skillID, int skillLevel);
    void ActiveSkill(int skillID, int skillLevel);
    void DeactiveSkill(int skillID);
    void DelBuffAllStackNum(CharacterBuff::Item &it);
    void CastOnce(int skillID, int skillLevel);
    void BindBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int skillID, int skillLevel);
    CharacterBuff::Item *GetBuffWithCompareFlag(int buffID, int buffLevel, int flag);
    CharacterBuff::Item *GetBuffByOwnerWithCompareFlag(int buffID, int buffLevel, int sourceID, int flag);
    std::tuple<int, int> CalcCritical(const CharacterAttr &attrSelf, int skillID, int skillLevel);
    int CalcDamage(const CharacterAttr &attrSelf, Character *target, DamageType typeDamage, bool isCritical, int atCriticalDamagePower, int DamageAddPercent, int damageBase, int damageRand, int nChannelInterval, int nWeaponDamagePercent);

    // ---------- 以下方法直接被 lua 调用 ----------
    bool IsFormationLeader();
    bool IsHaveBuff(int buffID, int buffLevel);
    CharacterBuff::Item *GetBuff(int buffID, int buffLevel);
    CharacterBuff::Item *GetBuffByOwner(int buffID, int buffLevel, int sourceID);
    int GetKungfuMountID();
    int GetSkillLevel(int skillID);
    int GetSkillTarget();
    void AddBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel);
    void CastSkill(int skillID, int skillLevel);
    void CastSkillTarget(int skillID, int skillLevel, int type, int targetID);
    void ModifyCoolDown(int cooldownID, int frame);
    void SetTimer3(int frame, std::string filename, int targetID);
    void SetTimer4(int frame, std::string filename, int type, int targetID);

    //  ---------- 被 lua 调用的属性, 通常以 "n" 开头 ----------
    int dwID;                          // 角色 ID
    int nLevel = chAttr.atLevel;       // 等级, 这是唯一一个同时存在于此处和 chAttr 内部的属性
    int nCurrentSunEnergy = 0;         // 当前日灵
    int nCurrentMoonEnergy = 0;        // 当前月魂
    int nSunPowerValue = 0;            // 满日
    int nMoonPowerValue = 0;           // 满月
    bool bSurplusAutoCast = false;     // 出现于 明教_套路_内功_焚影圣诀.lua
    bool bSurplusAutoReplenish = true; // 出现于 明教_套路_内功_焚影圣诀.lua

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