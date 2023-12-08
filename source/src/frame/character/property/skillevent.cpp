#include "frame/character/property/skillevent.h"

using namespace ns_frame;

void CharacterSkillEvent::add(int ID) {
    const SkillEvent &skillevent = SkillEventManager::get(ID);
    data[static_cast<int>(skillevent.type)].EventSkillID[skillevent.EventSkillID].insert(&skillevent);
    for (int i = 0; i < 32; i++) {
        if (skillevent.EventMask1 & (1 << i)) {
            data[static_cast<int>(skillevent.type)].EventMask1[i].insert(&skillevent);
        }
        if (skillevent.EventMask2 & (1 << i)) {
            data[static_cast<int>(skillevent.type)].EventMask2[i].insert(&skillevent);
        }
    }
}

void CharacterSkillEvent::remove(int ID) {
    const SkillEvent &skillevent = SkillEventManager::get(ID);
    data[static_cast<int>(skillevent.type)].EventSkillID[skillevent.EventSkillID].erase(&skillevent);
    for (int i = 0; i < 32; i++) {
        if (skillevent.EventMask1 & (1 << i)) {
            data[static_cast<int>(skillevent.type)].EventMask1[i].erase(&skillevent);
        }
        if (skillevent.EventMask2 & (1 << i)) {
            data[static_cast<int>(skillevent.type)].EventMask2[i].erase(&skillevent);
        }
    }
}

std::set<const SkillEvent *> CharacterSkillEvent::getList(ref::enumSkilleventEventtype type, int EventSkillID, uint32_t EventMask1, uint32_t EventMask2) {
    std::set<const SkillEvent *> emptySet;
    if (type == ref::enumSkilleventEventtype::COUNT) {
        return emptySet;
    }
    if (EventSkillID != 0) {
        emptySet.insert(data[static_cast<int>(type)].EventSkillID[EventSkillID].begin(), data[static_cast<int>(type)].EventSkillID[EventSkillID].end());
    }
    if (EventMask1 != 0) {
        for (int i = 0; i < 32; i++) {
            if (EventMask1 & (1 << i)) {
                emptySet.insert(data[static_cast<int>(type)].EventMask1[i].begin(), data[static_cast<int>(type)].EventMask1[i].end());
            }
        }
    }
    if (EventMask2 != 0) {
        for (int i = 0; i < 32; i++) {
            if (EventMask2 & (1 << i)) {
                emptySet.insert(data[static_cast<int>(type)].EventMask2[i].begin(), data[static_cast<int>(type)].EventMask2[i].end());
            }
        }
    }
    return emptySet;
}