#include "frame/event.h"

using namespace ns_frame;

bool EventManager::run() {
    if (data.empty()) {
        return false;
    }
    Event event = *data.begin();
    data.erase(data.begin());
    tick = event.tick;
    event.func(event.self, event.param);
    return true;
}

void EventManager::add(event_tick_t delay, event_func_t func, void *self, void *param) {
    data.emplace(tick + delay, func, self, param);
}