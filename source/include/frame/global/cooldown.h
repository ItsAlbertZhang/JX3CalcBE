#ifndef FRAME_GLOBAL_COOLDOWN_H_
#define FRAME_GLOBAL_COOLDOWN_H_

#include <mutex>
#include <string>
#include <unordered_map>

namespace ns_frame {

/**
 * @brief Cooldown 类
 * @warning Cooldown 是公共资源, 不属于某个角色. 应当使用 CooldownManager 类对其统一进行存取.
 * @warning #### 关于线程安全以及实现方式:
 * @warning - 这个类的实例会在多线程之间共享.
 * @warning - 在构造以外的情况下, 这个类的实例 **应当是** 只读的.
 * @warning - 这个类实例的构造由 `Manager::add()` 完成, 并借助锁确保线程安全.
 */
class Cooldown {
public:
    std::unordered_map<std::string, std::string> tab; // buffs.tab 中的数据

    // 初始化时拿出一些数据, 降低使用开销
    int ID;
    int DurationFrame;
    int MinDurationFrame;
    int MaxDurationFrame;
    int MaxCount;
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
    static const Cooldown &get(int ID);

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
