#pragma once

#include "frame/common/event.h"
#include <unordered_map>

namespace jx3calc {
namespace frame {

class ChCooldown {
public:
    /**
     * @brief 单个 Cooldown
     */
    class Item {
    public:
        int          countAvailable = 0; // 可用次数
        event_tick_t tickOverCurr   = 0; // 当前层冷却完毕的瞬间(时间点)
        event_tick_t tickOverAdd    = 0; // 非当前层的总冷却时间(时间段)
    };

    /**
     * @brief CD 列表
     * @note key 为 ID
     */
    std::unordered_map<int, Item> cooldownList;
};

} // namespace frame
} // namespace jx3calc
