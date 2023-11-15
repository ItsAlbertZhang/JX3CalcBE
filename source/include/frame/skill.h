#ifndef MAIN_FRAME_SKILL_H_
#define MAIN_FRAME_SKILL_H_

#include <unordered_map>

namespace ns_frame {

class Skill; // 前置声明

/**
 * 技能缓存类
 *
 * 这是一个静态类, 用于缓存技能数据, 不应当被创建实例.
 *
 * 计算器在计算技能时, 会先从此类中获取技能数据, 若此类中不存在技能数据, 则会从 GameDataFetcher 中获取技能数据, 并将其存至缓存.
 *
 * 在游戏中, 所有技能会在游戏开始时被加载. 而计算器不需要用到这么多的技能以及实现, 因此, 使用技能缓存类有助于提升计算器的开发效率与性能.
 *
 */
class CacheSkill {
public:
    CacheSkill() = delete; // 禁止创建实例

    /**
     * 技能缓存数据
     *
     * 是一个二维哈希表, 第一维的 key 为技能 ID, 第二维的 key 为技能等级, value 为技能数据.
     *
     * 同一 ID, 不同 Level 的技能拥有不同的 Skill 实例.
     */
    static std::unordered_map<int, std::unordered_map<int, Skill>> data;

    /**
     * @brief 初始化技能. 将指定 ID 与 Level 的技能数据存至缓存.
     *
     * @param skillID
     * @param skillLevel
     */
    static void init(int skillID, int skillLevel);

    /**
     * @brief 获取技能. 若技能存在, 则命中缓存并返回技能数据; 若技能不存在, 则对其进行初始化并返回技能数据.
     *
     * @param skillID
     * @param skillLevel
     * @return Skill&
     */
    static Skill &get(int skillID, int skillLevel);

private:
    /**
     * @brief 从 GameDataFetcher 中获取技能数据, 并存至缓存.
     *
     * @param skillID
     * @return Skill&
     * @note 在 skills.tab 中, 技能的存储不分等级, 因此此函数仅需一个参数 skillID.
     */
    static Skill &fetchData(int skillID);
};

/**
 * Skill 类
 *
 * 用于表示技能.
 *
 * 在平时的使用中, 一般不直接使用此类的方法, 而是使用 Character 类的接口.
 *
 */
class Skill {
public:
};

} // namespace ns_frame

#endif // MAIN_FRAME_SKILL_H_