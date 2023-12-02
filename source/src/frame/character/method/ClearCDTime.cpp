#include "frame/character/character.h"
#include "frame/event.h"

using namespace ns_frame;

void Character::ClearCDTime(int cooldownID) {
    if (this->chCooldown.cooldownList.find(cooldownID) == this->chCooldown.cooldownList.end()) {
        return;
    }
    CharacterCooldown::Item &item = this->chCooldown.cooldownList.at(cooldownID);
    if (item.isValid) {
        int frame = static_cast<int>((Event::now() - item.tickOver) / 64 + 1);
        this->ModifyCoolDown(cooldownID, frame);
    }
}