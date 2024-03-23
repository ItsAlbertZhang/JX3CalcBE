#ifndef FRAME_GLOBAL_SKILLEVENT_H_
#define FRAME_GLOBAL_SKILLEVENT_H_

#include "frame/ref/global_skillevent.h"
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>

namespace jx3calc {
namespace frame {

/**
 * @brief SkillEvent 类
 * @warning SkillEvent 是公共资源, 不属于某个角色. 应当使用 SkillEventManager 类对其统一进行存取.
 * @warning #### 关于线程安全以及实现方式:
 * @warning - 这个类的实例会在多线程之间共享.
 * @warning - 在构造以外的情况下, 这个类的实例 **应当是** 只读的.
 * @warning - 这个类实例的构造由 `Manager::add()` 完成, 并借助锁确保线程安全.
 */
class SkillEvent {
public:
    std::unordered_map<std::string, std::string> tab; // buffs.tab 中的数据

    // 初始化时拿出一些数据, 降低使用开销
    int                             ID;
    ref::enumSkilleventEventtype    type = ref::enumSkilleventEventtype::COUNT;
    int                             Odds;
    int                             SkillID;
    int                             SkillLevel;
    ref::enumSkilleventCastertarget SkillCaster = ref::enumSkilleventCastertarget::COUNT;
    ref::enumSkilleventCastertarget SkillTarget = ref::enumSkilleventCastertarget::COUNT;
    uint32_t                        EventMask1;
    uint32_t                        EventMask2;
    int                             EventSkillID;
    // int                             EventSkillLevel; // 此属性事实上被弃用
};

/**
 * @brief SkillEvent 管理类
 * @warning 这是一个静态类, 用于管理 SkillEvent 数据, 不应当被创建实例.
 */
class SkillEventManager {
public:
    SkillEventManager() = delete;

    /**
     * @brief 获取 SkillEvent. 若 SkillEvent 存在, 则命中缓存并返回 SkillEvent 数据; 若 SkillEvent 不存在, 则对其进行初始化并返回 SkillEvent 数据.
     * @param ID
     * @return SkillEvent&
     */
    static const SkillEvent &get(int ID);

private:
    static inline std::mutex mutex; // 互斥锁. 用于保护 add 操作.

    /**
     * @brief SkillEvent 缓存数据
     */
    static inline std::unordered_map<int, SkillEvent> data;

    /**
     * @brief 初始化 SkillEvent. 将指定 ID 的 SkillEvent 数据存至缓存.
     * @param ID
     */
    static void add(int ID);
};

} // namespace frame
} // namespace jx3calc

#endif // FRAME_GLOBAL_SKILLEVENT_H_
