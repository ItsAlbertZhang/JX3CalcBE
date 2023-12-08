#include "frame/global/skillevent.h"
#include "frame/character/character.h"

using namespace ns_frame;

void Character::skilleventAdd(int eventID) {
    const SkillEvent &skillevent = SkillEventManager::get(eventID);
    this->chSkillEvent.data[static_cast<int>(skillevent.type)].EventSkillID[skillevent.EventSkillID].insert(&skillevent);
    for (int i = 0; i < 32; i++) {
        if (skillevent.EventMask1 & (1 << i)) {
            this->chSkillEvent.data[static_cast<int>(skillevent.type)].EventMask1[i].insert(&skillevent);
        }
        if (skillevent.EventMask2 & (1 << i)) {
            this->chSkillEvent.data[static_cast<int>(skillevent.type)].EventMask2[i].insert(&skillevent);
        }
    }
}

void Character::skilleventRemove(int eventID) {
    const SkillEvent &skillevent = SkillEventManager::get(eventID);
    this->chSkillEvent.data[static_cast<int>(skillevent.type)].EventSkillID[skillevent.EventSkillID].erase(&skillevent);
    for (int i = 0; i < 32; i++) {
        if (skillevent.EventMask1 & (1 << i)) {
            this->chSkillEvent.data[static_cast<int>(skillevent.type)].EventMask1[i].erase(&skillevent);
        }
        if (skillevent.EventMask2 & (1 << i)) {
            this->chSkillEvent.data[static_cast<int>(skillevent.type)].EventMask2[i].erase(&skillevent);
        }
    }
}

std::set<const SkillEvent *> Character::skilleventGet(ref::enumSkilleventEventtype type, int eventskillID, uint32_t eventmask1, uint32_t eventmask2) {
    std::set<const SkillEvent *> emptySet;
    if (type == ref::enumSkilleventEventtype::COUNT) {
        return emptySet;
    }
    if (eventskillID != 0) {
        emptySet.insert(this->chSkillEvent.data[static_cast<int>(type)].EventSkillID[eventskillID].begin(), this->chSkillEvent.data[static_cast<int>(type)].EventSkillID[eventskillID].end());
    }
    if (eventmask1 != 0) {
        for (int i = 0; i < 32; i++) {
            if (eventmask1 & (1 << i)) {
                emptySet.insert(this->chSkillEvent.data[static_cast<int>(type)].EventMask1[i].begin(), this->chSkillEvent.data[static_cast<int>(type)].EventMask1[i].end());
            }
        }
    }
    if (eventmask2 != 0) {
        for (int i = 0; i < 32; i++) {
            if (eventmask2 & (1 << i)) {
                emptySet.insert(this->chSkillEvent.data[static_cast<int>(type)].EventMask2[i].begin(), this->chSkillEvent.data[static_cast<int>(type)].EventMask2[i].end());
            }
        }
    }
    return emptySet;
}