#include "frame/global/skillrecipe.h"
#include "frame/runtime_lua.h"
#include "gdi.h"

using namespace ns_frame;

const SkillRecipe &SkillRecipeManager::get(int RecipeID, int RecipeLevel) {
    // 若 SkillRecipe ID 不存在, 则添加
    if (data.find(std::make_tuple(RecipeID, RecipeLevel)) == data.end()) {
        add(RecipeID, RecipeLevel);
    }
    return data[std::make_tuple(RecipeID, RecipeLevel)];
}

void SkillRecipeManager::add(int RecipeID, int RecipeLevel) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断 SkillRecipe 是否存在.
    if (data.find(std::make_tuple(RecipeID, RecipeLevel)) != data.end()) {
        // 若 Recipe ID 存在, 且 Recipe 等级存在, 则直接返回
        return; // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }

    // 初始化 Cooldown
    SkillRecipe skillrecipe;
    skillrecipe.RecipeID = RecipeID;
    skillrecipe.RecipeLevel = RecipeLevel;
    // 获取 tab
    gdi::TabSelectType arg;
    arg.emplace_back();
    arg[0]["RecipeID"] = std::to_string(RecipeID);
    arg[0]["RecipeLevel"] = std::to_string(RecipeLevel);
    gdi::Interface::tabSelect(gdi::Tab::skillrecipe, arg);
    skillrecipe.tab = std::move(arg[0]);
    // 初始化数据. std::stoi() 用于确定字段存在的情况. 若该字段可能为空, 必须使用 atoi().
    skillrecipe.SkillRecipeType = atoi(skillrecipe.tab["SkillRecipeType"].c_str());
    skillrecipe.SkillID = atoi(skillrecipe.tab["SkillID"].c_str());
    skillrecipe.CoolDownAdd1 = atoi(skillrecipe.tab["CoolDownAdd1"].c_str());
    skillrecipe.CoolDownAdd2 = atoi(skillrecipe.tab["CoolDownAdd2"].c_str());
    skillrecipe.CoolDownAdd3 = atoi(skillrecipe.tab["CoolDownAdd3"].c_str());
    skillrecipe.DamageAddPercent = atoi(skillrecipe.tab["DamageAddPercent"].c_str());
    // 将 Cooldown 存入缓存
    data[std::make_tuple(RecipeID, RecipeLevel)] = std::move(skillrecipe);
    SkillRecipe *curr = &data[std::make_tuple(RecipeID, RecipeLevel)];

    Skill skill;
    // 执行 GetSkillRecipeData
    std::string path = skillrecipe.tab["ScriptFile"];
    if (!path.empty()) {
        std::string name = "scripts/skill/" + path;
        bool res = LuaFunc::analysis(LuaFunc::getGetSkillRecipeData(name)(skill, RecipeID, RecipeLevel), name, LuaFunc::Enum::GetSkillRecipeData);
        if (res) {
            // 成功执行, 将技能添加到 ScriptSkill 中
            ScriptSkill[curr] = std::move(skill);
        }
    }
}