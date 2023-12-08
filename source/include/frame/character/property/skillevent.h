#ifndef FRAME_CHARACTER_PROPERTY_SKILLEVENT_H_
#define FRAME_CHARACTER_PROPERTY_SKILLEVENT_H_

#include "frame/global/skillevent.h"
#include <set>
#include <unordered_map>

namespace ns_frame {

class CharacterSkillEvent {
public:
    void add(int ID);
    void remove(int ID);
    std::set<const SkillEvent *> getList(ref::enumSkilleventEventtype type, int EventSkillID, uint32_t EventMask1, uint32_t EventMask2);

private:
    class ItemType {
    public:
        std::unordered_map<int, std::set<const SkillEvent *>> EventSkillID; // 用于快速查找指定 SkillID 的 SkillEvent
        std::set<const SkillEvent *> EventMask1[32];                        // 用于快速查找指定 EventMask1 的 SkillEvent
        std::set<const SkillEvent *> EventMask2[32];                        // 用于快速查找指定 EventMask2 的 SkillEvent
    };
    ItemType data[static_cast<int>(ref::enumSkilleventEventtype::COUNT)];
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_SKILLEVENT_H_