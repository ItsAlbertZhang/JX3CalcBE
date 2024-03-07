#ifndef FRAME_EVENT_H_
#define FRAME_EVENT_H_

#include <cstdint>
#include <set>
#include <tuple>

namespace ns_frame {

using event_tick_t = int64_t;
using event_func_t = void (*)(void *, void *);

/**
 * @brief Event 管理类
 * @warning 这是一个静态类, 用于管理 event-time (事件-时间), 不应当被创建实例.
 * @warning 这个类通过静态数据成员 thread_local 的形式实现线程安全.
 */
class Event {
public:
    Event() = delete;

    static bool run();

    /**
     * @brief 添加一个事件
     * @param delay 生效延迟, 为生效 tick 与 当前 tick 的差值
     * @param func 回调函数
     * @param self 回调函数的第一个参数
     * @param param 回调函数的第二个参数
     * @retval 该事件的生效 tick
     */
    static event_tick_t add(event_tick_t delay, event_func_t func, void *self, void *param);

    /**
     * @brief 移除一个事件
     * @param tick 该事件的生效 tick
     * @param func 该事件的回调函数
     * @param self 该事件的回调函数的第一个参数
     * @param param 该事件的回调函数的第二个参数
     * @retval 原本的生效延迟, 即该事件的生效 tick 与 当前 tick 的差值
     */
    static event_tick_t cancel(event_tick_t tick, event_func_t func, void *self, void *param);

    /**
     * @return 当前 tick
     */
    static event_tick_t now();

    /**
     * @brief 清空事件列表
     */
    static void clear();

private:
    static inline thread_local event_tick_t tick;

    class EventItem {
    public:
        EventItem(event_tick_t tick, event_func_t func, void *self, void *param)
            : tick(tick), func(func), self(self), param(param) {}
        event_tick_t tick;  // 生效时间
        event_func_t func;  // 回调函数
        void        *self;  // 回调函数的第一个参数
        void        *param; // 回调函数的第二个参数
        bool         operator<(const EventItem &other) const {
            return std::tie(tick, func, self, param) < std::tie(other.tick, other.func, other.self, other.param);
        }
    };
    static inline thread_local std::set<EventItem> eventList;
};

} // namespace ns_frame

#endif // FRAME_EVENT_H_
