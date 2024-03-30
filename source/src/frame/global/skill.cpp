#include "frame/global/skill.h"
#include "frame/lua/interface.h"
#include "gdi.h"
#include "plugin/log.h"

#define UNREFERENCED_PARAMETER(P) (P)

using namespace jx3calc;
using namespace frame;

const Skill &SkillManager::get(int skillID, int skillLevel) {
    // 若技能 ID 不存在, 则添加
    if (data.find(skillID) == data.end()) {
        add(skillID, skillLevel);
    } else {
        // 若技能 ID 存在, 但技能等级不存在, 则添加
        if (data[skillID].find(skillLevel) == data[skillID].end()) {
            add(skillID, skillLevel);
        }
    }
    return data[skillID][skillLevel];
}

void SkillManager::add(int skillID, int skillLevel) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断技能是否存在.
    if (data.find(skillID) != data.end() && data[skillID].find(skillLevel) != data[skillID].end()) {
        // 若技能 ID 存在, 且技能等级存在, 则直接返回
        return; // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }
    // 初始化技能
    Skill skill;
    skill.dwSkillID = skillID;
    skill.dwLevel   = skillLevel;
    // 获取 tab
    if (data.find(skillID) == data.end()) {
        // 如果没有该技能 ID, 则先获取 tab
        gdi::select_t arg;
        arg.emplace_back();
        arg[0]["SkillID"] = std::to_string(skillID);
        gdi::tabSelect(gdi::Tab::skills, arg);
        skill.tab = std::move(arg[0]);
    } else {
        // 如果该技能 ID 已存在, 则复用同 ID 技能的 tab
        auto it   = data[skillID].begin();
        skill.tab = it->second.tab;
    }
    // 初始化数据. std::stoi() 用于确定字段存在的情况. 若该字段可能为空, 必须使用 atoi().
    skill.KindType             = Ref<ref::Skill::KindType>::map.at(skill.tab["KindType"]);
    skill.IsPassiveSkill       = skill.tab["IsPassiveSkill"] == "1";
    skill.HasCriticalStrike    = skill.tab["HasCriticalStrike"] == "1";
    skill.SkillEventMask1      = atoi(skill.tab["SkillEventMask1"].c_str());
    skill.SkillEventMask2      = atoi(skill.tab["SkillEventMask2"].c_str());
    skill.NeedOutOfFight       = skill.tab["NeedOutOfFight"] == "1";
    skill.TargetTypePlayer     = skill.tab["TargetTypePlayer"] == "1";
    skill.TargetTypeNpc        = skill.tab["TargetTypeNpc"] == "1";
    skill.TargetRelationNone   = skill.tab["TargetRelationNone"] == "1";
    skill.TargetRelationSelf   = skill.tab["TargetRelationSelf"] == "1";
    skill.TargetRelationEnemy  = skill.tab["TargetRelationEnemy"] == "1";
    skill.RecipeType           = atoi(skill.tab["RecipeType"].c_str());
    // 处理默认武器伤害.
    // 目前推测: WeaponRequest 字段非 0 的技能默认拥有 1024 的武器伤害 (可以在后续 lua 的 getGetSkillLevelData 中被覆盖).
    // 注意: 拥有武器伤害不一定代表会造成武器伤害. 造成武器伤害与 AddAttribute 中的 CALL_PHYSICS_DAMAGE 有关.
    // 推测的依据:
    // 1. 部分技能并没有在 lua 中显式声明 nWeaponDamagePercent, 但是仍然可以造成武器伤害. (最简单的例子即为普通攻击)
    // 2. 部分不造成武器伤害的外功技能, 似乎都在 lua 中显式声明了其 nWeaponDamagePercent = 0. (例如, 丐帮的诸多需要武器施展的技能.)
    // 暂时按照该推测进行处理.
    skill.nWeaponDamagePercent = !skill.tab["WeaponRequest"].empty() && skill.tab["WeaponRequest"] != "0" ? 1024 : 0;
    // 执行 GetSkillLevelData
    std::string name           = "scripts/skill/" + skill.tab["ScriptFile"];
    bool        res            = lua::interface::analysis(
        lua::interface::getGetSkillLevelData(name)(skill), name, lua::interface::FuncType::GetSkillLevelData
    );

    if (res) {
        // 查询技能 UI
        gdi::select_t arg;
        arg.emplace_back();
        arg[0]["SkillID"] = std::to_string(skillID);
        arg[0]["Level"]   = std::to_string(skillLevel);
        arg.emplace_back();
        arg[1]["SkillID"] = std::to_string(skillID);
        arg[1]["Level"]   = "0";
        gdi::tabSelect(gdi::Tab::ui_skill, arg);
        if (arg.size() == 0) {
            skill.Name = "未知技能";
        } else {
            skill.ui   = std::move(arg[0]);
            skill.Name = skill.ui["Name"];
        }
        // 将技能存入缓存
        data[skillID][skillLevel] = std::move(skill);
    } else {
        CONSTEXPR_LOG_ERROR("LuaFunc::getGetSkillLevelData(\"{}\") failed.", name);
    }
}

void Skill::SetDelaySubSkill(int a, int b, int c) {
    attrDelaySubSkill.emplace_back(a, b, c);
}

void Skill::AddAttribute(int a, int b, int c, int d) {
    attrAttributes.emplace_back(a, b, c, d);
}

void Skill::AddAttribute(int a, int b, double c, int d) {
    attrAttributes.emplace_back(a, b, c, d);
}

void Skill::AddAttribute(int a, int b, std::string c, int d) {
    attrAttributes.emplace_back(a, b, c, d);
}

void Skill::AddAttribute(int a, int b, std::optional<char> nil, int d) {
    // std::optional<char> 其实是 nil
    // 出现于 scripts/skill/江湖/110级CW新增特效焚影伤害子技能.lua, 原代码如下:
    /*
tSkillData =
{
    {nDamageBase= 20, nDamageRand = 2, nCostMana = 0}, --level 1
};
...
    skill.AddAttribute(
        ATTRIBUTE_EFFECT_MODE.EFFECT_TO_SELF_AND_ROLLBACK,
        ATTRIBUTE_TYPE.SKILL_LUNAR_DAMAGE,
        tSkillData[dwSkillLevel].nDamage, // 注意这里不是 nDamageBase 而是 nDamage
        0
    );
    */
    // 只能说是非常离谱
    UNREFERENCED_PARAMETER(nil);
    attrAttributes.emplace_back(a, b, 0, d);
}

void Skill::AddSlowCheckSelfBuff(int a, int b, int c, int d, int e) {
    attrCheckBuff.emplace_back(SkillCheckBuff::TypeEnum::self, a, b, c, d, e);
}

void Skill::AddSlowCheckDestBuff(int a, int b, int c, int d, int e) {
    attrCheckBuff.emplace_back(SkillCheckBuff::TypeEnum::dest, a, b, c, d, e);
}

void Skill::AddSlowCheckSelfOwnBuff(int a, int b, int c, int d, int e) {
    attrCheckBuff.emplace_back(SkillCheckBuff::TypeEnum::selfOwn, a, b, c, d, e);
}

void Skill::AddSlowCheckDestOwnBuff(int a, int b, int c, int d, int e) {
    attrCheckBuff.emplace_back(SkillCheckBuff::TypeEnum::destOwn, a, b, c, d, e);
}

void Skill::AddCheckSelfLearntSkill(int a, int b, int c) {
    attrCheckSelfLearntSkill.emplace_back(a, b, c);
}

void Skill::BindBuff(int a, int b, int c) {
    attrBindBuff.used              = true;
    attrBindBuff.isValid[a - 1]    = true;
    attrBindBuff.nBuffID[a - 1]    = b;
    attrBindBuff.nBuffLevel[a - 1] = c;
}

void Skill::SetPublicCoolDown(int a) {
    attrCoolDown.used                  = true;
    attrCoolDown.isValidPublicCoolDown = true;
    attrCoolDown.nPublicCoolDown       = a;
}

void Skill::SetNormalCoolDown(int a, int b) {
    attrCoolDown.used                         = true;
    attrCoolDown.isValidNormalCoolDown[a - 1] = true;
    attrCoolDown.nNormalCoolDownID[a - 1]     = b;
}

void Skill::SetCheckCoolDown(int a, int b) {
    attrCoolDown.used                        = true;
    attrCoolDown.isValidCheckCoolDown[a - 1] = true;
    attrCoolDown.nCheckCoolDownID[a - 1]     = b;
}

void Skill::SetSubsectionSkill(int a, int b, int c, int d) {
    UNREFERENCED_PARAMETER(a);
    UNREFERENCED_PARAMETER(b);
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(d);
    CONSTEXPR_LOG_ERROR("SetSubsectionSkill 未实现!{}", "");
}

void Skill::SetSunSubsectionSkill(int a, int b, int c, int d) {
    UNREFERENCED_PARAMETER(a);
    UNREFERENCED_PARAMETER(b);
    SunSubsectionSkillID    = c;
    SunSubsectionSkillLevel = d;
}

void Skill::SetMoonSubsectionSkill(int a, int b, int c, int d) {
    UNREFERENCED_PARAMETER(a);
    UNREFERENCED_PARAMETER(b);
    MoonSubsectionSkillID    = c;
    MoonSubsectionSkillLevel = d;
}
