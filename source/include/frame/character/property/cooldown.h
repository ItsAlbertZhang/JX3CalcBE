#ifndef FRAME_CHARACTER_PROPERTY_COOLDOWN_H_
#define FRAME_CHARACTER_PROPERTY_COOLDOWN_H_

#include "frame/common/event.h"
#include <unordered_map>

namespace ns_frame {

class ChCooldown {
public:
    /**
     * @brief 单个 Cooldown
     */
    class Item {
    public:
        int          countAvailable = 0; // 可用次数
        event_tick_t tickOver       = 0; // 距离当前层冷却完毕的 tick
        event_tick_t tickAdditional = 0; // 非当前层的总冷却 tick
    };

    /**
     * @brief CD 列表
     * @note key 为 ID
     */
    std::unordered_map<int, Item> cooldownList;
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_COOLDOWN_H_
