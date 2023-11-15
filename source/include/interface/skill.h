#ifndef MAIN_INTERFACE_SKILL_H_
#define MAIN_INTERFACE_SKILL_H_

#include <iostream>
#include <string>

namespace ns_interface {

// 暂时定义, 以后会被删除.
class Skill {
public:
    int func() {
        printf("in function, skill is %p\n", this);
        return 0;
    }
};

// 技能接口.
class InterfaceSkill {
public:
    static Skill *current_skill; // 暂时指向一个临时定义的 Skill 对象, 以后应当指向 ns_frame/skill.h 中的 Skill 对象.
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