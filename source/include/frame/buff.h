#ifndef FRAME_BUFF_H_
#define FRAME_BUFF_H_

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace ns_frame {

/**
 * Buff 类
 * 注意: Buff 是公共资源, 不属于某个角色. 应当使用 BuffManager 类对其统一进行存取.
 */
class Buff {
public:
    // ---------- 数据存放区 ----------
    std::unordered_map<std::string, std::string> tab; // buffs.tab 中的数据
    std::vector<std::string> BeginAttrib;
    std::vector<std::string> ActiveAttrib;
    std::vector<std::string> EndTimeAttrib;
};

/**
 * @brief Buff 管理类
 * 这是一个静态类, 用于管理 Buff 数据, 不应当被创建实例.
 */
class BuffManager {
public:
    BuffManager() = delete;

    /**
     * @brief 获取 Buff. 若 Buff 存在, 则命中缓存并返回 Buff 数据; 若 Buff 不存在, 则对其进行初始化并返回 Buff 数据.
     * @param buffID
     * @param buffLevel
     * @return Buff&
     */
    static Buff &get(int buffID, int buffLevel);

private:
    static std::mutex mutex; // 互斥锁. 用于保护 SkillManager::add 操作.

    /**
     * Buff 缓存数据
     * 同一 ID, 不同 Level 的 Buff 拥有不同的 Buff 实例.
     */
    static std::unordered_map<int, std::unordered_map<int, Buff>> data;

    /**
     * @brief 初始化技能. 将指定 ID 与 Level 的 Buff 数据存至缓存.
     * @param buffID
     * @param buffLevel
     */
    static void add(int buffID, int buffLevel);
};

} // namespace ns_frame

#endif // FRAME_BUFF_H_