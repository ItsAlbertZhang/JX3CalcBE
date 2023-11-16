#include "frame/character.h"
#include "frame/interface.h"
#include "frame/skill.h"

using namespace ns_frame;

int Character::nextCharacterID = 0;

Character::Character() {
    nCharacterID = nextCharacterID++;
}

void Character::LearnSkill(int skillID, int skillLevel) {
    skillLearned[skillID] = skillLevel;
    SkillManager::get(skillID, skillLevel);
}

void Character::CastSkill(int skillID, int skillLevel) {
    // 获取技能
    Skill &skill = SkillManager::get(skillID, skillLevel);
    // test, 遍历 Attribute
    int current_attribute_string_index = 0;
    for (auto &it : skill.attributes) {
        if (it.type >= 0) {
            std::cout << ns_interface::luaAttributeEffectMode[it.mode] << "\n"
                      << ns_interface::luaAttributeType[it.type] << "\n"
                      << it.param1 << "\t" << it.param2 << "\n"
                      << std::endl;
        } else {
            auto &it_string = skill.attributesString[current_attribute_string_index++];
            std::cout << ns_interface::luaAttributeEffectMode[it_string.mode] << "\n"
                      << ns_interface::luaAttributeType[it_string.type] << "\n"
                      << it_string.param1 << "\t" << it_string.param2 << "\n"
                      << std::endl;
        }
    }

    // switch (type) {
    // case static_cast<int>(LuaGlobalTable::ATTRIBUTE_TYPE::CAST_SKILL_TARGET_DST):
    //     std::cout << "CAST_SKILL_TARGET_DST" << std::endl;
    // default:
    //     std::cerr << "位置类型:\t" << type << "\t" << LuaGlobalTable::AttributeType[type] << std::endl;
    // }
}

void Character::AddBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel) {
}