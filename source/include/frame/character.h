#ifndef FRAME_CHARACTER_H_
#define FRAME_CHARACTER_H_

#include "frame/character_attribute.h"
#include "frame/character_buff.h"
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

namespace ns_frame {
/**
 * @brief Character 类
 * @note 用于表示游戏中的角色.
 * @note 在平时的使用中, 基本上更多使用此类提供的接口, 而不直接操作此类的成员变量的方法.
 */
class Character {
public:
    Character(); // 构造函数

    int nCharacterID;         // 角色 ID
    bool isPlayer = false;    // 是否为玩家
    Character *target = this; // 当前目标
    CharacterAttr attribute;  // 角色属性

    std::unordered_map<int, int> skillLearned; // 已学习技能列表. key 为技能 ID, value 为技能等级.

    class SkillQueueElement { // 技能队列元素
    public:
        SkillQueueElement(int skillID, int skillLevel) : skillID(skillID), skillLevel(skillLevel) {}
        int skillID;    // 技能 ID
        int skillLevel; // 技能等级
    };
    std::queue<SkillQueueElement> skillQueue; // 待执行的技能队列.

    std::unordered_map<int, std::unordered_map<int, CharacterBuff>> buffExist; // 已存在的 buff 列表. key1 为 Buff ID, key2 为 Buff Level.

    // ---------- 以下方法直接被 lua 调用 ----------
    void LearnSkill(int skillID, int skillLevel);
    void CastSkill(int skillID, int skillLevel);
    void AddBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel);

    //  ---------- 不被 CharacterAttr 包含的属性, 可能被 lua 调用, 以 "n" 开头而非 "at" ----------
    int nLevel = 120;                               // 等级
    int nSpunkToSolarAndLunarAttackPowerCof = 0;    // 元气转换为阳性和阴性内功攻击的系数
    int nSpunkToSolarAndLunarCriticalStrikeCof = 0; // 元气转换为阳性和阴性内功会心的系数

    int nMaxSunEnergy = 0;  // 最大日灵
    int nMaxMoonEnergy = 0; // 最大月魂

private:
    static std::vector<Character *> characterList; // 角色列表
    bool hasBuff(int buffID, int buffLevel);       // 检查是否存在指定的 buff
    static bool luaBuffCompare(int flag, int luaValue, int buffValue);
};

class Player : public Character {
};

class NPC : public Character {
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_H_