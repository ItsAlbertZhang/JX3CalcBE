#ifndef MAIN_FRAME_SKILL_ATTRIBUTE_H_
#define MAIN_FRAME_SKILL_ATTRIBUTE_H_

#include <string>

namespace ns_frame {

class SkillAttribute {
public:
    SkillAttribute(int mode, int type, int param1, int param2)
        : mode(mode), type(type), param1(param1), param2(param2) {}
    int mode;
    int type;
    int param1;
    int param2;
};

class SkillAttributeString {
public:
    SkillAttributeString(int mode, int type, std::string param1, int param2)
        : mode(mode), type(type), param1(param1), param2(param2) {}
    int mode;
    int type;
    std::string param1;
    int param2;
};

enum class CheckBuffEnum {
    self,
    dest,
    selfOwn,
    destOwn,
};

class CheckBuff {
public:
    CheckBuff(CheckBuffEnum type, int dwBuffID, int nStackNum, int nStackNumCompareFlag, int nLevel, int nLevelCompareFlag)
        : type(type), dwBuffID(dwBuffID), nStackNum(nStackNum), nStackNumCompareFlag(nStackNumCompareFlag), nLevel(nLevel), nLevelCompareFlag(nLevelCompareFlag) {}
    CheckBuffEnum type;
    int dwBuffID;
    int nStackNum;
    int nStackNumCompareFlag;
    int nLevel;
    int nLevelCompareFlag;
};

class CheckSelfLearntSkill {
public:
    CheckSelfLearntSkill(int dwSkillID, int dwSkillLevel, int LevelCompareFlag)
        : dwSkillID(dwSkillID), dwSkillLevel(dwSkillLevel), LevelCompareFlag(LevelCompareFlag) {}
    int dwSkillID;
    int dwSkillLevel;
    int LevelCompareFlag;
};

class BindBuff {
public:
    BindBuff(int index, int nBuffID, int nBuffLevel)
        : index(index), nBuffID(nBuffID), nBuffLevel(nBuffLevel) {}
    int index;
    int nBuffID;
    int nBuffLevel;
};

enum class CoolDownEnum {
    publicCD,
    normalCD,
    checkCD,
};

class CoolDown {
public:
    CoolDown(CoolDownEnum type, int index, int nCoolDownID)
        : type(type), index(index), nCoolDownID(nCoolDownID) {}
    CoolDownEnum type;
    int index;
    int nCoolDownID;
};

} // namespace ns_frame

#endif // MAIN_FRAME_SKILL_ATTRIBUTE_H_