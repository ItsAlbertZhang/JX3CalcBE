#ifndef FRAME_CHARACTER_PROPERTY_BUFF_H_
#define FRAME_CHARACTER_PROPERTY_BUFF_H_

#include "frame/character/property/attribute.h"
#include "frame/common/event.h"
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace ns_frame {

class BuffItem {
public:
    BuffItem(
        int nCharacterID,
        int rawInterval,
        int rawCount,
        int nID,
        int nLevel,
        int nIndex,
        int dwSkillSrcID
    )
        : nCharacterID(nCharacterID), rawInterval(rawInterval), rawCount(rawCount),
          nID(nID), nLevel(nLevel), nIndex(nIndex), dwSkillSrcID(dwSkillSrcID) {}

    const int    nCharacterID = 0; // 角色 ID
    const int    rawInterval  = 0;
    const int    rawCount     = 0;
    bool         isValid      = false;
    event_tick_t tickActive   = 0; // 下一次生效的 tick
    int          interval     = 0; // 生效间隔 (单位: 帧). 该属性非必要, 因其可以通过 attr 计算得出. 但为了避免每次 active 时都需要调用 BuffManager::get(), 故在此保存一份.

    void  *ptrAttrib = nullptr; // 自动回滚的 buff 属性, 用于处理 buff 的 BeginAttrib, ActiveAttrib, EndTimeAttrib
    ChAttr attr;                // 保存一份属性的副本, 用于快照

    // buff
    const int nID              = 0;
    const int nLevel           = 0;
    const int nIndex           = 0;
    int       nStackNum        = 0; // 层数
    int       nLeftActiveCount = 0; // 剩余生效次数
    int       nLeftFrame       = 0; // 剩余帧数
    int       nCustomValue     = 0; // 自定义值
    int       nNextActiveFrame = 0; // 下一次生效的帧数

    // 来源
    const int dwSkillSrcID       = 0; // 来源角色 ID, 注意与来源技能 ID 区分.
    int       dwCasterSkillID    = 0; // 来源技能 ID, 注意与来源角色 ID 区分.
    int       dwCasterSkillLevel = 0; // 来源技能等级
    int       nChannelInterval   = 0; // 单跳系数
};

class ChBuff {
public:
    /**
     * @brief buff 列表
     * @note key 为三层嵌套: sourceID -> buffID -> buffLevel
     * @note 为最大化查找效率, 使用此结构.
     * @note 第三层使用 map 而非 unordered_map 是因为 buffGetWithCompareFlag 中需要使用 lower_bound 方法.
     */
    std::unordered_map<int, std::unordered_map<int, std::map<int, BuffItem>>> buffMap;  // buff 列表, 用于查找
    std::vector<BuffItem *>                                                   buffList; // buff 列表, 用于遍历
    std::unordered_multimap<std::string, BuffItem *>                          buffRef;  // buff 反查表, buff 名称 - BuffItem.

    /**
     * @brief 按插入时间排序的 buff 列表
     * @note 游戏内的真实实现大概率采用此结构. 在处理 buff 的卸除时, 需要使用有序的列表.
     * @note 计算器并不需要此逻辑. 未来需要使用时, 再做相关实现.
     */
    // std::multimap<event_tick_t, Item *> buffTickList;
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_BUFF_H_
