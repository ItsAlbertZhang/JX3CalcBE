#ifndef FRAME_CHARACTER_PROPERTY_BUFF_H_
#define FRAME_CHARACTER_PROPERTY_BUFF_H_

#include <unordered_map>

namespace ns_frame {

class Character;

class CharacterBuff {
public:
    /**
     * @brief 单个 buff
     */
    class Item {
    public:
        bool isValid = false;
        using event_tick_t = unsigned long long;
        event_tick_t tickOver = 0; // 结束 tick

        Character *source = nullptr;
        int nStackNum = 0;

        // OnRemove: nCharacterID, BuffID, nBuffLevel, nLeftFrame, nCustomValue, dwSkillSrcID, nStackNum, nBuffIndex, dwCasterID, dwCasterSkillID
    };

    struct tuple_hash {
        template <class T1, class T2>
        std::size_t operator()(const std::tuple<T1, T2> &t) const {
            auto h1 = std::hash<T1>{}(std::get<0>(t));
            auto h2 = std::hash<T2>{}(std::get<1>(t));
            return h1 ^ h2;
        }
    };

    std::unordered_map<std::tuple<int, int>, Item, tuple_hash> buffList;
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_BUFF_H_