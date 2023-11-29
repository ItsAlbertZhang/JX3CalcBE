#ifndef FRAME_CHARACTER_PROPERTY_BUFF_H_
#define FRAME_CHARACTER_PROPERTY_BUFF_H_

#include "frame/character/property/attribute.h"
#include <map>
#include <unordered_map>

namespace ns_frame {

using event_tick_t = unsigned long long;

class CharacterBuff {
public:
    /**
     * @brief 单个 buff
     */
    class Item {
    public:
        Item(int nCharacterID, int dwSkillSrcID, int BuffID, int nBuffLevel)
            : nCharacterID(nCharacterID), dwSkillSrcID(dwSkillSrcID), BuffID(BuffID), nBuffLevel(nBuffLevel) {}
        const int nCharacterID = 0; // 角色 ID
        const int dwSkillSrcID = 0; // 来源角色 ID, 注意不是技能 ID
        const int BuffID = 0;
        const int nBuffLevel = 0;

        int nLeftFrame = 0;
        int nCustomValue = 0;
        int nStackNum = 0;

        bool isValid = false;
        event_tick_t tickActive = 0; // 下一次生效的 tick
        int interval = 0;            // 生效间隔 (单位: 帧). 该属性非必要, 因其可以通过 attr 计算得出. 但为了避免每次 active 时都需要调用 BuffManager::get(), 故在此保存一份.
        int count = 0;               // 剩余生效次数

        void *ptrAttrib = nullptr; // 自动回滚的 buff 属性, 用于处理 buff 的 BeginAttrib, ActiveAttrib, EndTimeAttrib
        CharacterAttr attr;        // 保存一份属性的副本, 用于快照

        // 以下属性只有 BindBuff 才拥有
        int dwCasterSkillID = 0;    // 来源技能 ID
        int dwCasterSkillLevel = 0; // 来源技能等级
        int nChannelInterval = 0;   // 单跳系数

        // OnRemove: nCharacterID, BuffID, nBuffLevel, nLeftFrame, nCustomValue, dwSkillSrcID, nStackNum, [[nBuffIndex, dwCasterID, dwCasterSkillID]] 未实现
    };

    /**
     * @brief buff 列表
     * @note key 为三层嵌套: sourceID -> buffID -> buffLevel
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