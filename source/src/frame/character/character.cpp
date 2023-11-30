#include "frame/character/character.h"

using namespace ns_frame;

Character::Character() {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    this->dwID = static_cast<int>(characterList.size());
    characterList.push_back(this);
    characterMap.emplace(this, dwID);
}
