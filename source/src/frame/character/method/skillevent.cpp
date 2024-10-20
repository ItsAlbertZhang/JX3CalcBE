#include "frame/global/skillevent.h"
#include "frame/character/character.h"
#include <algorithm>
#include <ranges>

using namespace jx3calc;
using namespace frame;

void Character::skilleventAdd(int eventID) {
    const SkillEvent &skillevent = SkillEventManager::get(eventID);
    auto             &vec        = this->chSkillEvent[static_cast<int>(skillevent.type)];
    if (std::ranges::find(vec, &skillevent) == vec.end()) {
        vec.emplace_back(&skillevent);
    }
}

void Character::skilleventRemove(int eventID) {
    const SkillEvent &skillevent = SkillEventManager::get(eventID);
    auto             &vec        = this->chSkillEvent[static_cast<int>(skillevent.type)];
    auto              it         = std::ranges::find(vec, &skillevent); // iterator
    if (it != vec.end()) {
        vec.erase(it);
    }
}

bool Character::skilleventExist(int eventID) {
    const SkillEvent &skillevent = SkillEventManager::get(eventID);
    auto             &vec        = this->chSkillEvent[static_cast<int>(skillevent.type)];
    return std::ranges::find(vec, &skillevent) != vec.end();
}

std::vector<const SkillEvent *> Character::skilleventGet(
    ref::SkillEvent::EventType type, int eventskillID, uint32_t eventmask1, uint32_t eventmask2
) {
    // std::set<const SkillEvent *> emptySet;
    // if (eventskillID != 0) {
    //     emptySet.insert(
    //         this->chSkillEvent.data[static_cast<int>(type)].EventSkillID[eventskillID].begin(),
    //         this->chSkillEvent.data[static_cast<int>(type)].EventSkillID[eventskillID].end()
    //     );
    // }
    // if (eventmask1 != 0) {
    //     for (int i = 0; i < 32; i++) {
    //         if (eventmask1 & (1 << i)) {
    //             emptySet.insert(
    //                 this->chSkillEvent.data[static_cast<int>(type)].EventMask1[i].begin(),
    //                 this->chSkillEvent.data[static_cast<int>(type)].EventMask1[i].end()
    //             );
    //         }
    //     }
    // }
    // if (eventmask2 != 0) {
    //     for (int i = 0; i < 32; i++) {
    //         if (eventmask2 & (1 << i)) {
    //             emptySet.insert(
    //                 this->chSkillEvent.data[static_cast<int>(type)].EventMask2[i].begin(),
    //                 this->chSkillEvent.data[static_cast<int>(type)].EventMask2[i].end()
    //             );
    //         }
    //     }
    // }
    // return emptySet;

    auto &vec     = this->chSkillEvent[static_cast<int>(type)];
    bool  reverse = type == ref::SkillEvent::EventType::PreCast; // reverse if PreCast

    std::vector<const SkillEvent *> ret;
    auto                            back = std::back_inserter(ret);

    auto lambda = [ret, eventskillID, eventmask1, eventmask2](const SkillEvent *skillevent) {
        if (std::ranges::find(ret, skillevent) != ret.end()) {
            return false;
        }
        auto isMatched =
            (eventskillID != 0 && skillevent->EventSkillID == eventskillID) ||
            (eventmask1 != 0 && (skillevent->EventMask1 & eventmask1) != 0) ||
            (eventmask2 != 0 && (skillevent->EventMask2 & eventmask2) != 0);
        return isMatched;
    };

    if (reverse)
        std::ranges::copy_if(vec | std::ranges::views::reverse, back, lambda);
    else
        std::ranges::copy_if(vec, back, lambda);

    return ret;
}
