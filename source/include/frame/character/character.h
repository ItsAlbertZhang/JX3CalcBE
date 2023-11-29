#ifndef FRAME_CHARACTER_H_
#define FRAME_CHARACTER_H_

#include "frame/character/property/attribute.h"
#include "frame/character/property/buff.h"
#include "frame/character/property/cooldown.h"
#include "frame/character/property/damage.h"
#include "frame/character/property/skill.h"
#include "frame/character/property/skillrecipe.h"
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace ns_frame {
/**
 * @brief Character 类
 * @note 用于表示游戏中的角色.
 * @warning #### 关于线程安全:
 * @warning 这个类的实例会在线程内部被创建. 因此, 其天然就是线程安全的.
 */
class Character {
public:
    Character(); // 构造函数

    bool isPlayer = false;    // 是否为玩家
    Character *target = this; // 当前目标
    bool isOutOfFight = true; // 是否处于战斗状态

    CharacterAttr chAttr;               // 角色属性
    CharacterBuff chBuff;               // 角色 buff
    CharacterCooldown chCooldown;       // 角色冷却
    CharacterDamage chDamage;           // 角色伤害
    CharacterSkill chSkill;             // 角色技能
    CharacterSkillRecipe chSkillRecipe; // 角色技能秘籍

    // ---------- 以下方法未被 lua 调用 ----------
    static Character *getCharacter(int nCharacterID);
    static int getCharacterID(Character *character);
    void LearnSkill(int skillID, int skillLevel);
    void ActiveSkill(int skillID, int skillLevel);
    void DeactiveSkill(int skillID);
    void DelBuffAllStackNum(CharacterBuff::Item &it);
    void BindBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel, int skillID, int skillLevel);
    CharacterBuff::Item *GetBuffWithCompareFlag(int buffID, int buffLevel, int flag);
    CharacterBuff::Item *GetBuffByOwnerWithCompareFlag(int buffID, int buffLevel, int sourceID, int flag);
    std::tuple<int, int> CalcCritical(const CharacterAttr &attrSelf, int skillID, int skillLevel);
    int CalcDamage(const CharacterAttr &attrSelf, Character *target, DamageType typeDamage, int damageBase, int damageRand, bool isCritical, int atCriticalDamagePower, int nChannelInterval, int nWeaponDamagePercent);

    // ---------- 以下方法直接被 lua 调用 ----------
    bool IsHaveBuff(int buffID, int buffLevel);
    CharacterBuff::Item *GetBuff(int buffID, int buffLevel);
    CharacterBuff::Item *GetBuffByOwner(int buffID, int buffLevel, int sourceID);
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
    bool bSurplusAutoCast = false;     // 出现于 明教_套路_内功_焚影圣诀.lua
    bool bSurplusAutoReplenish = true; // 出现于 明教_套路_内功_焚影圣诀.lua

private:
    static inline std::vector<Character *> characterList;            // 角色列表
    static inline std::unordered_map<Character *, int> characterMap; // 角色表
};

class Player : public Character {
};

class NPC : public Character {
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_H_