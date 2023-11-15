#include "interface/lua_skill.h"
#include <iostream>

using namespace ns_interface;

thread_local ns_frame::Skill *InterfaceSkill::current_skill = nullptr;

void InterfaceSkill::AddAttribute_iiii(int a, int b, int c, int d) {
    InterfaceSkill::current_skill->attributes.emplace_back(a, b, c, d);
}

void InterfaceSkill::AddAttribute_iidi(int a, int b, double c, int d) {
    AddAttribute_iiii(a, b, (int)c, d);
}

void InterfaceSkill::AddAttribute_iisi(int a, int b, std::string c, int d) {
    InterfaceSkill::current_skill->attributes.emplace_back(-1, -1, -1, -1); // 占位
    InterfaceSkill::current_skill->attributesString.emplace_back(a, b, c, d);
}

void InterfaceSkill::AddSlowCheckSelfBuff(int a, int b, int c, int d, int e) {
    std::cout << "AddSlowCheckSelfBuff"
              << "\t# " << a << "\t# " << b << "\t# " << c << "\t# " << d << "\t# " << e
              << std::endl;
}

void InterfaceSkill::AddSlowCheckDestBuff(int a, int b, int c, int d, int e) {
    std::cout << "AddSlowCheckDestBuff"
              << "\t# " << a << "\t# " << b << "\t# " << c << "\t# " << d << "\t# " << e
              << std::endl;
}

void InterfaceSkill::AddSlowCheckSelfOwnBuff(int a, int b, int c, int d, int e) {
    std::cout << "AddSlowCheckSelfOwnBuff"
              << "\t# " << a << "\t# " << b << "\t# " << c << "\t# " << d << "\t# " << e
              << std::endl;
}

void InterfaceSkill::AddSlowCheckDestOwnBuff(int a, int b, int c, int d, int e) {
    std::cout << "AddSlowCheckDestOwnBuff"
              << "\t# " << a << "\t# " << b << "\t# " << c << "\t# " << d << "\t# " << e
              << std::endl;
}

void InterfaceSkill::AddCheckSelfLearntSkill(int a, int b, int c) {
    std::cout << "AddCheckSelfLearntSkill"
              << "\t# " << a << "\t# " << b << "\t# " << c << std::endl;
}

void InterfaceSkill::BindBuff(int a, int b, int c) {
    std::cout << "BindBuff"
              << "\t# " << a << "\t# " << b << "\t# " << c << std::endl;
}

void InterfaceSkill::SetPublicCoolDown(int a) {
    std::cout << "SetPublicCoolDown"
              << "\t# " << a << std::endl;
}

void InterfaceSkill::SetNormalCoolDown(int a, int b) {
    std::cout << "SetNormalCoolDown"
              << "\t# " << a << "\t# " << b << std::endl;
}

void InterfaceSkill::SetCheckCoolDown(int a, int b) {
    std::cout << "SetCheckCoolDown"
              << "\t# " << a << "\t# " << b << std::endl;
}

void InterfaceSkill::SetSubsectionSkill(int a, int b, int c, int d) {
    std::cerr << "SetSubsectionSkill 未实现!" << std::endl;
}