#ifndef FRAME_CHARACTER_H_
#define FRAME_CHARACTER_H_

#include "frame/character/property/attribute.h"
#include "frame/character/property/buff.h"
#include "frame/character/property/cooldown.h"
#include "frame/character/property/skill.h"
#include <string>
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

    CharacterAttr chAttr;         // 角色属性
    CharacterSkill chSkill;       // 角色技能
    CharacterBuff chBuff;         // 角色 buff
    CharacterCooldown chCooldown; // 角色冷却

    // ---------- 以下方法未被 lua 调用 ----------
    static Character *getCharacter(int nCharacterID);
    void LearnSkill(int skillID, int skillLevel);

    // ---------- 以下方法直接被 lua 调用 ----------
    bool IsHaveBuff(int buffID, int buffLevel);
    CharacterBuff::Item *GetBuff(int buffID, int buffLevel);
    int GetSkillLevel(int skillID);
    void AddBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel);
    void CastSkill(int skillID, int skillLevel);
    void CastSkillTarget(int skillID, int skillLevel, int type, int targetID);
    void ModifyCoolDown(int cooldownID, int frame);
    void SetTimer(int frame, std::string filename, int type, int targetID);

    //  ---------- 被 lua 调用的属性, 通常以 "n" 开头 ----------
    int dwID;         // 角色 ID
    int nLevel = 120; // 等级

    int nSpunkToSolarAndLunarAttackPowerCof = 0;    // 元气转换为阳性和阴性内功攻击的系数
    int nSpunkToSolarAndLunarCriticalStrikeCof = 0; // 元气转换为阳性和阴性内功会心的系数

    int nMaxSunEnergy = 0;      // 最大日灵
    int nMaxMoonEnergy = 0;     // 最大月魂
    int nCurrentSunEnergy = 0;  // 当前日灵
    int nCurrentMoonEnergy = 0; // 当前月魂

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