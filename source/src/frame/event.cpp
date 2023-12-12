#include "frame/event.h"

using namespace ns_frame;

bool Event::run() {
    if (eventList.empty()) {
        return false;
    }
    EventItem event = *eventList.begin();
    eventList.erase(eventList.begin());
    tick = event.tick;
    event.func(event.self, event.param);
    return true;
}

event_tick_t Event::add(event_tick_t delay, event_func_t func, void *self, void *param) {
    eventList.emplace(tick + delay, func, self, param);
    return tick + delay;
}

event_tick_t Event::cancel(event_tick_t tick, event_func_t func, void *self, void *param) {
    eventList.erase(EventItem(tick, func, self, param));
    return tick - Event::tick;
}

event_tick_t Event::now() {
    return tick;
}

void Event::clear() {
    eventList.clear();
    tick = 0;
}
