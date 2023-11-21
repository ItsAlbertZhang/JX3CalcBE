#ifndef FRAME_GLOBAL_COOLDOWN_H_
#define FRAME_GLOBAL_COOLDOWN_H_

#include <mutex>
#include <string>
#include <unordered_map>

namespace ns_frame {

/**
 * @brief Cooldown 类
 * @warning Cooldown 是公共资源, 不属于某个角色. 应当使用 CooldownManager 类对其统一进行存取.
 */
class Cooldown {
public:
    std::unordered_map<std::string, std::string> tab; // buffs.tab 中的数据
};

/**
 * @brief Cooldown 管理类
 * @warning 这是一个静态类, 用于管理 Cooldown 数据, 不应当被创建实例.
 */
class CooldownManager {
public:
    CooldownManager() = delete;

    /**
     * @brief 获取 Cooldown. 若 Cooldown 存在, 则命中缓存并返回 Cooldown 数据; 若 Cooldown 不存在, 则对其进行初始化并返回 Cooldown 数据.
     * @param ID
     * @return Cooldown&
     */
    static Cooldown &get(int ID);

private:
    static inline std::mutex mutex; // 互斥锁. 用于保护 add 操作.

    /**
     * @brief Cooldown 缓存数据
     */
    static inline std::unordered_map<int, Cooldown> data;

    /**
     * @brief 初始化 Cooldown. 将指定 ID 的 Cooldown 数据存至缓存.
     * @param ID
     */
    static void add(int ID);
};

} // namespace ns_frame

#endif // FRAME_GLOBAL_COOLDOWN_H_