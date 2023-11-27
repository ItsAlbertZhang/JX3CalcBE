#ifndef FRAME_CHARACTER_PROPERTY_BUFF_H_
#define FRAME_CHARACTER_PROPERTY_BUFF_H_

#include "frame/character/property/attribute.h"
#include <map>
#include <unordered_map>

namespace ns_frame {

class Character;
using event_tick_t = unsigned long long;

class CharacterBuff {
public:
    /**
     * @brief 单个 buff
     */
    class Item {
    public:
        bool isValid = false;
        event_tick_t tickOver = 0; // 结束 tick

        Character *source = nullptr;
        CharacterAttr attr; // 保存一份属性的副本, 用于快照

        int nStackNum = 0;

        // OnRemove: nCharacterID, BuffID, nBuffLevel, nLeftFrame, nCustomValue, dwSkillSrcID, nStackNum, nBuffIndex, dwCasterID, dwCasterSkillID
    };

    /**
     * @brief buff 列表
     * @note 三层嵌套: sourceID -> buffID -> buffLevel
     * @note 为最大化查找效率, 使用此结构.
     */
    std::unordered_map<int, std::unordered_map<int, std::map<int, Item>>> buffList;

    /**
     * @brief 按插入时间排序的 buff 列表
     * @note 游戏内的真实实现大概率采用此结构. 在处理 buff 的卸除时, 需要使用有序的列表.
     * @note 计算器并不需要此逻辑. 未来需要使用时, 再做相关实现.
     */
    // std::multimap<event_tick_t, Item *> buffTickList;
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_BUFF_H_