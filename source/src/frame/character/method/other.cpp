#include "frame/character/character.h"

#define UNREFERENCED_PARAMETER(P) (P)

using namespace jx3calc;
using namespace frame;

int Character::otherGetMapID() {
    return 0;
}

int Character::otherGetKungfuMountID() {
    return this->kungfuID;
}

bool Character::otherIsFormationLeader() {
    return false; // TODO: 暂时未实现阵眼功能
}

void Character::otherPlayPublicShadowAnimation(int a, int b, bool c, bool d) {
    UNREFERENCED_PARAMETER(a);
    UNREFERENCED_PARAMETER(b);
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(d);
    return;
}

ChScene *Character::sceneGet() {
    return &(this->chScene);
}

void Character::otherClearAllNpcThreat() {
    return;
}

bool Character::otherIsInParty() {
    return false;
}

void Character::otherDoAction(int a, int b) {
    UNREFERENCED_PARAMETER(a);
    UNREFERENCED_PARAMETER(b);
    return;
}

void Character::otherDestroyPublicShadow(int a, bool b) {
    UNREFERENCED_PARAMETER(a);
    UNREFERENCED_PARAMETER(b);
    return;
}

void Character::otherCreatePublicShadow(int a, int roletype, int id, int x, int y, int z, bool b) {
    UNREFERENCED_PARAMETER(a);
    UNREFERENCED_PARAMETER(roletype);
    UNREFERENCED_PARAMETER(id);
    UNREFERENCED_PARAMETER(x);
    UNREFERENCED_PARAMETER(y);
    UNREFERENCED_PARAMETER(z);
    UNREFERENCED_PARAMETER(b);
    return;
}
