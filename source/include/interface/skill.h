#ifndef MAIN_INTERFACE_SKILL_H_
#define MAIN_INTERFACE_SKILL_H_

#include "frame/skill.h"
#include <iostream>
#include <string>

namespace ns_interface {

// 技能接口.
class InterfaceSkill {
public:
    static ns_frame::Skill *current_skill;
    static void AddAttribute_iiii(int a, int b, int c, int d);
    static void AddAttribute_iisi(int a, int b, std::string c, int d);
    static void AddAttribute_iidi(int a, int b, double c, int d);
    static void AddSlowCheckSelfBuff(int a, int b, int c, int d, int e);
    static void BindBuff(int a, int b, int c);
    static void SetPublicCoolDown(int a);
    static void SetNormalCoolDown(int a, int b);
    static void SetCheckCoolDown(int a, int b);
    int dwLevel = 1;
    int nCostSprintPower;
    int nMinRadius;
    int nMaxRadius;
    int nAngleRange;
    bool bFullAngleInAir;
    double nChannelInterval;
    int nWeaponDamagePercent;
};

} // namespace ns_interface

#endif // MAIN_INTERFACE_SKILL_H_