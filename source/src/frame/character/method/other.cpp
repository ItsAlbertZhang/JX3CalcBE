#include "frame/character/character.h"

using namespace ns_frame;

int Character::otherGetMapID() {
    return 0;
}

int Character::otherGetKungfuMountID() {
    return this->dwKungfuID;
}

bool Character::otherIsFormationLeader() {
    return false; // TODO: 暂时未实现阵眼功能
}

void Character::otherPlayPublicShadowAnimation(int a, int b, bool c, bool d) {
    return;
}

CharacterScene *Character::sceneGet() {
    return &(this->chScene);
}

void Character::otherClearAllNpcThreat() {
    return;
}

bool Character::otherIsInParty() {
    return false;
}

void Character::otherDoAction(int a, int b) {
    return;
}

void Character::otherDestroyPublicShadow(int a, bool b) {
    return;
}

void Character::otherCreatePublicShadow(int a, int roletype, int dwID, int nX, int nY, int nZ, bool b) {
    return;
}
