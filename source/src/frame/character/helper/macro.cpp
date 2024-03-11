#include "frame/character/property/buff.h"
#include <frame/character/character.h>

using namespace ns_frame;

int Character::macroSkillRef(std::string skillName) {
    if (!chSkill.skillRef.contains(skillName))
        return -1;
    return chSkill.skillRef.at(skillName);
}

int Character::macroBuff(std::string buffName) {
    if (!chBuff.buffRef.contains(buffName))
        return 0;
    auto range = chBuff.buffRef.equal_range(buffName);
    if (range.first == range.second)
        return 0;
    for (auto it = range.first; it != range.second; ++it) {
        BuffItem *item = it->second;
        if (item->isValid) {
            return item->nStackNum;
        }
    }
    return 0;
}
bool Character::macroNoBuff(std::string buffName) {
    return 0 == macroBuff(buffName);
}
double Character::macroBufftime(std::string buffName) {
    if (!chBuff.buffRef.contains(buffName))
        return -1.0;
    auto range = chBuff.buffRef.equal_range(buffName);
    if (range.first == range.second)
        return -1.0;
    for (auto it = range.first; it != range.second; ++it) {
        BuffItem *item = it->second;
        if (item->isValid) {
            buffFlushLeftFrame(item);
            return static_cast<double>(item->nLeftFrame) / 16;
        }
    }
    return -1.0;
}
int Character::macroTBuff(std::string buffName) {
    return this->targetSelect->macroBuff(buffName);
}
bool Character::macroTNoBuff(std::string buffName) {
    return this->targetSelect->macroNoBuff(buffName);
}
double Character::macroTBufftime(std::string buffName) {
    return this->targetSelect->macroBufftime(buffName);
}
int Character::macroSun() {
    return nCurrentSunEnergy / 100;
}
int Character::macroMoon() {
    return nCurrentMoonEnergy / 100;
}
