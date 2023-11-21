#ifndef FRAME_CHARACTER_H_
#define FRAME_CHARACTER_H_

#include "frame/character/property/attribute.h"
#include "frame/character/property/buff.h"
#include "frame/character/property/cooldown.h"
#include "frame/character/property/skill.h"
#include <vector>

namespace ns_frame {
/**
 * @brief Character 类
 * @note 用于表示游戏中的角色.
 */
class Character {
public:
    Character(); // 构造函数

    int nCharacterID;         // 角色 ID
    bool isPlayer = false;    // 是否为玩家
    Character *target = this; // 当前目标

    CharacterAttr chAttr;         // 角色属性
    CharacterSkill chSkill;       // 角色技能
    CharacterBuff chBuff;         // 角色 buff
    CharacterCooldown chCooldown; // 角色冷却

    // ---------- 以下方法暂未确定是否被 lua 调用 ----------
    void LearnSkill(int skillID, int skillLevel);
    bool hasBuff(int buffID, int buffLevel); // 检查是否存在指定的 buff

    // ---------- 以下方法直接被 lua 调用 ----------
    void CastSkill(int skillID, int skillLevel);
    void AddBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel);

    //  ---------- 不被 CharacterAttr 包含的属性, 可能被 lua 调用, 以 "n" 开头而非 "at" ----------
    int nLevel = 120;                               // 等级
    int nSpunkToSolarAndLunarAttackPowerCof = 0;    // 元气转换为阳性和阴性内功攻击的系数
    int nSpunkToSolarAndLunarCriticalStrikeCof = 0; // 元气转换为阳性和阴性内功会心的系数

    int nMaxSunEnergy = 0;  // 最大日灵
    int nMaxMoonEnergy = 0; // 最大月魂

private:
    static inline std::vector<Character *> characterList; // 角色列表
};

class Player : public Character {
};

class NPC : public Character {
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_H_