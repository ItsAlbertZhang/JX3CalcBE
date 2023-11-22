#ifndef FRAME_EVENT_H_
#define FRAME_EVENT_H_

#include <set>

namespace ns_frame {

using event_tick_t = unsigned long long;
using event_func_t = void (*)(void *);

/**
 * @brief Event 类
 * @warning Event 是公共资源, 不属于某个角色. 应当使用 EventManager 类对其统一进行存取.
 * @warning #### 关于线程安全以及实现方式:
 * @warning - 这个类的实例不会在多线程之间共享.
 * @warning - 这个类实例的构造由 `Manager::add()` 完成, 并借助 thread_local 确保线程安全.
 */
class Event {
public:
    Event(event_tick_t tick, event_func_t func, void *param)
        : tick(tick), func(func), param(param) {}
    event_tick_t tick; // 生效时间
    event_func_t func; // 回调函数
    void *param;       // 回调函数参数
    bool operator<(const Event &other) const {
        return tick < other.tick;
    }
};

/**
 * @brief Event 管理类
 * @warning 这是一个静态类, 用于管理 event-time (事件-时间), 不应当被创建实例.
 * @warning 这个类通过静态数据成员 thread_local 的形式实现线程安全.
 */
class EventManager {
public:
    EventManager() = delete;

    bool run();
    void add(event_tick_t delay, event_func_t func, void *param);

private:
    static inline thread_local event_tick_t tick;
    static inline thread_local std::multiset<Event> data;
};

} // namespace ns_frame

#endif // FRAME_EVENT_H_