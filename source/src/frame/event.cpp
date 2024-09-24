#include "frame/event.h"

using namespace jx3calc;
using namespace frame;

bool Event::run() {
    if (eventList.empty()) {
        return false;
    }
    EventItem event = *eventList.begin(); // 复制构造, func, self, param 都是指针, 不会有问题
    eventList.erase(eventList.begin());   // 删除迭代器指向的元素
    tick = event.tick;
    event.func(event.self, event.param);
    return true;
}

event_tick_t Event::add(event_tick_t delay, event_func_t func, void *self, void *param) {
    eventList.emplace(tick + delay, func, self, param);
    return tick + delay;
}

event_tick_t Event::cancel(event_tick_t active, event_func_t func, void *self, void *param) {
    // eventList.erase(EventItem(active, func, self, param));
    auto range = eventList.equal_range(EventItem(active, func, self, param));
    for (auto it = range.first; it != range.second; ++it) { // 无需再次判断 tick 是否相等
        if (it->func == func && it->self == self && it->param == param) {
            eventList.erase(it);
            break;
        }
    }
    return active - tick;
}

event_tick_t Event::now() {
    return tick;
}

void Event::clear() {
    eventList.clear();
    tick = 0;
}
