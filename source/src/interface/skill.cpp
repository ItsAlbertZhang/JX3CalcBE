#include "interface/skill.h"

using namespace ns_interface;

ns_frame::Skill *InterfaceSkill::current_skill = nullptr;

void InterfaceSkill::AddAttribute_iiii(int a, int b, int c, int d) {
    std::cout << "iiii"
              << "\t# " << a << "\t# " << b << "\t# " << c << "\t# " << d << std::endl;
}

void InterfaceSkill::AddAttribute_iisi(int a, int b, std::string c, int d) {
    std::cout << "iisi"
              << "\t# " << a << "\t# " << b << "\t# " << c << "\t# " << d << std::endl;
}

void InterfaceSkill::AddAttribute_iidi(int a, int b, double c, int d) {
    std::cout << "iidi"
              << "\t# " << a << "\t# " << b << "\t# " << c << "\t# " << d << std::endl;
}

void InterfaceSkill::AddSlowCheckSelfBuff(int a, int b, int c, int d, int e) {
    std::cout << "AddSlowCheckSelfBuff"
              << "\t# " << a << "\t# " << b << "\t# " << c << "\t# " << d << "\t# " << e
              << std::endl;
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