#pragma once

#include "frame/ref/skillevent.h"
#include <set>
#include <unordered_map>

namespace jx3calc {
namespace frame {

class SkillEvent;

class ChSkillEvent {
public:
    // 下列方法均已在 Character 类中实现.
    // void add(int ID);
    // void remove(int ID);
    // std::set<const SkillEvent *> getList(ref::SkillEvent::EventType type, int EventSkillID, uint32_t EventMask1, uint32_t EventMask2);

    class ItemType {
    public:
        std::unordered_map<int, std::set<const SkillEvent *>> EventSkillID;   // 用于快速查找指定 SkillID 的 SkillEvent
        std::set<const SkillEvent *>                          EventMask1[32]; // 用于快速查找指定 EventMask1 的 SkillEvent
        std::set<const SkillEvent *>                          EventMask2[32]; // 用于快速查找指定 EventMask2 的 SkillEvent
    };

    ItemType data[Ref<ref::SkillEvent::EventType>::count];
};

} // namespace frame
} // namespace jx3calc
