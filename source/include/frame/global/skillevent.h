#ifndef FRAME_GLOBAL_SKILLEVENT_H_
#define FRAME_GLOBAL_SKILLEVENT_H_

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>

namespace ns_frame {

enum class EventType {
    BeCast,
    BeCriticalStrike,
    BeDodge,
    BeHit,
    BeHitOTAction,
    BeKill,
    BeMiss,
    BeOverHeal,
    BlockLongRange,
    Cast,
    CriticalStrike,
    Dodge,
    Hit,
    HitOTAction,
    Kill,
    Miss,
    OverHeal,
    Parry,
    PreCast,
    COUNT, // 用于计数以及表明出错
};

enum class EventCT {
    EventCaster,
    EventTarget,
    COUNT, // 用于计数以及表明出错
};

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
    int ID;
    EventType type = EventType::COUNT;
    int Odds;
    int SkillID;
    int SkillLevel;
    EventCT SkillCaster = EventCT::COUNT;
    EventCT SkillTarget = EventCT::COUNT;
    uint32_t EventMask1;
    uint32_t EventMask2;
    int EventSkillID;
    // int EventSkillLevel; // 此属性事实上被弃用
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
    static inline std::unordered_map<EventType, std::unordered_map<uint32_t, std::vector<SkillEvent *>>> EventMask1Map;
    static inline std::unordered_map<EventType, std::unordered_map<uint32_t, std::vector<SkillEvent *>>> EventMask2Map;
    static inline std::unordered_map<EventType, std::unordered_map<int, std::vector<SkillEvent *>>> EventSkillIDMap;

    /**
     * @brief 初始化 SkillEvent. 将指定 ID 的 SkillEvent 数据存至缓存.
     * @param ID
     */
    static void add(int ID);

    static inline const std::unordered_map<std::string, EventType> EventTypeMap = {
        {"BeCast", EventType::BeCast},
        {"BeCriticalStrike", EventType::BeCriticalStrike},
        {"BeDodge", EventType::BeDodge},
        {"BeHit", EventType::BeHit},
        {"BeHitOTAction", EventType::BeHitOTAction},
        {"BeKill", EventType::BeKill},
        {"BeMiss", EventType::BeMiss},
        {"BeOverHeal", EventType::BeOverHeal},
        {"BlockLongRange", EventType::BlockLongRange},
        {"Cast", EventType::Cast},
        {"CriticalStrike", EventType::CriticalStrike},
        {"Dodge", EventType::Dodge},
        {"Hit", EventType::Hit},
        {"HitOTAction", EventType::HitOTAction},
        {"Kill", EventType::Kill},
        {"Miss", EventType::Miss},
        {"OverHeal", EventType::OverHeal},
        {"Parry", EventType::Parry},
        {"PreCast", EventType::PreCast},
    };
    static inline const std::unordered_map<std::string, EventCT> EventCTMap = {
        {"EventCaster", EventCT::EventCaster},
        {"EventTarget", EventCT::EventTarget},
    };
};

} // namespace ns_frame

#endif // FRAME_GLOBAL_SKILLEVENT_H_