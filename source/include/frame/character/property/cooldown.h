#ifndef FRAME_CHARACTER_PROPERTY_COOLDOWN_H_
#define FRAME_CHARACTER_PROPERTY_COOLDOWN_H_

#include <unordered_map>

namespace ns_frame {

class Character;
using event_tick_t = unsigned long long;

class CharacterCooldown {
public:
    /**
     * @brief 单个 Cooldown
     */
    class Item {
    public:
        bool isValid = false;
        event_tick_t tickOver = 0; // 结束 tick
    };

    std::unordered_map<int, Item> cooldownList;
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_COOLDOWN_H_