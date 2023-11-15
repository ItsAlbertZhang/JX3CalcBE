#ifndef MAIN_FRAME_CHARACTER_H_
#define MAIN_FRAME_CHARACTER_H_

#include <unordered_map>

namespace ns_frame {

/**
 * Character 类
 *
 * 用于表示游戏中的角色.
 *
 * 在平时的使用中, 基本上更多使用此类提供的接口, 而不直接操作此类的成员变量的方法.
 *
 */
class Character {
public:
    Character(); // 构造函数

    // 一些比较确定的实现
    int nCharacterID;      // 角色 ID
    bool isPlayer = false; // 是否为玩家

    // 一些连蒙带猜的实现
    int nTargetID;                             // 目标 ID
    std::unordered_map<int, int> skillLearned; // 已学习技能列表. key 为技能 ID, value 为技能等级

    // 一些连蒙带猜的实现

    void setTarget(int targetID); // 设置目标 ID

    // 一些比较确定的实现. 此处不做多态的实现, lua 调用中的多态在 ns_interface 模块中实现.

    void LearnSkill(int skillID, int skillLevel);
    void CastSkill(int skillID, int skillLevel);
    void AddBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel);

private:
    static int nextCharacterID;
};

class Player : public Character {
};

class NPC : public Character {
};

} // namespace ns_frame

#endif // MAIN_FRAME_CHARACTER_H_