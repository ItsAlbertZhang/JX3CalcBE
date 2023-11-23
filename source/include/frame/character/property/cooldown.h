#ifndef FRAME_CHARACTER_PROPERTY_COOLDOWN_H_
#define FRAME_CHARACTER_PROPERTY_COOLDOWN_H_

#include <unordered_map>

namespace ns_frame {

class Character;

class CharacterCooldown {
public:
    /**
     * @brief 单个 Cooldown
     */
    class Item {
    public:
        bool isValid = false;
        int tickOver = 0; // 结束 tick
    };

    std::unordered_map<int, Item> cooldownList;
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_COOLDOWN_H_