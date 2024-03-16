#ifndef FRAME_GLOBAL_ITEM_H_
#define FRAME_GLOBAL_ITEM_H_

#include "frame/common/item.h"
#include <array>
#include <mutex>
#include <string>
#include <unordered_map>
namespace ns_frame {

/**
 * @brief Item 类
 * @warning Item 是公共资源, 不属于某个角色. 应当使用 CooldownManager 类对其统一进行存取.
 * @warning #### 关于线程安全以及实现方式:
 * @warning - 这个类的实例会在多线程之间共享.
 * @warning - 在构造以外的情况下, 这个类的实例 **应当是** 只读的.
 * @warning - 这个类实例的构造由 `Manager::add()` 完成, 并借助锁确保线程安全.
 */
class Item {
public:
    std::unordered_map<std::string, std::string> tab; // tab 中的数据

    // 初始化时拿出一些数据, 降低使用开销
    ItemType type;
    int      ID;
    int      SkillID;
    int      SkillLevel;
    int      CoolDownID;
};

/**
 * @brief Item 管理类
 * @warning 这是一个静态类, 用于管理 Item 数据, 不应当被创建实例.
 */
class ItemManager {
public:
    ItemManager() = delete;

    /**
     * @brief 获取 Item. 若 Item 存在, 则命中缓存并返回 Item 数据; 若 Item 不存在, 则对其进行初始化并返回 Item 数据.
     * @param ID
     * @return Item&
     */
    static const Item &get(ItemType type, int ID);

private:
    static inline std::mutex mutex; // 互斥锁. 用于保护 add 操作.

    /**
     * @brief Item 缓存数据
     */
    static inline std::array<std::unordered_map<int, Item>, static_cast<size_t>(ItemType::COUNT)> data;

    /**
     * @brief 初始化 Item. 将指定 ItemType 和 ID 的 Item 数据存至缓存.
     * @param ID
     */
    static void add(ItemType type, int ID);
};

} // namespace ns_frame

#endif // FRAME_GLOBAL_ITEM_H_
