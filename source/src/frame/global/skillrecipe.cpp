#include "frame/global/skillrecipe.h"
#include "frame/lua_runtime.h"
#include "gdi.h"
#include "global/constexpr_log.h"
#include <tuple>

using namespace ns_frame;

const SkillRecipe &SkillRecipeManager::getRecipe(int RecipeID, int RecipeLevel) {
    // 若不存在, 则添加
    if (mapRecipe.find(std::make_tuple(RecipeID, RecipeLevel)) == mapRecipe.end()) {
        add(RecipeID, RecipeLevel);
    }
    return mapRecipe[std::make_tuple(RecipeID, RecipeLevel)];
}

const Skill *SkillRecipeManager::getScriptSkill(const SkillRecipe *skillrecipe, const Skill *skill) {
    if (!skillrecipe->hasScriptFile || skillrecipe == nullptr || skill == nullptr) {
        return nullptr;
    }
    if (mapScriptSkill.find(std::make_tuple(skillrecipe, skill)) == mapScriptSkill.end()) {
        // 若不存在, 则添加
        addScriptSkill(skillrecipe, skill);
    }
    return &mapScriptSkill[std::make_tuple(skillrecipe, skill)];
}

void SkillRecipeManager::add(int RecipeID, int RecipeLevel) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断 SkillRecipe 是否存在.
    if (mapRecipe.find(std::make_tuple(RecipeID, RecipeLevel)) != mapRecipe.end()) {
        // 若 Recipe ID && Recipe 等级存在, 则直接返回
        return; // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }

    // 初始化 Cooldown
    SkillRecipe skillrecipe;
    skillrecipe.RecipeID    = RecipeID;
    skillrecipe.RecipeLevel = RecipeLevel;
    // 获取 tab
    gdi::select_t arg;
    arg.emplace_back();
    arg[0]["RecipeID"]    = std::to_string(RecipeID);
    arg[0]["RecipeLevel"] = std::to_string(RecipeLevel);
    gdi::tabSelect(gdi::Tab::skillrecipe, arg);
    skillrecipe.tab                                   = std::move(arg[0]);
    // 初始化数据. std::stoi() 用于确定字段存在的情况. 若该字段可能为空, 必须使用 atoi().
    skillrecipe.SkillRecipeType                       = atoi(skillrecipe.tab["SkillRecipeType"].c_str());
    skillrecipe.SkillID                               = atoi(skillrecipe.tab["SkillID"].c_str());
    skillrecipe.CoolDownAdd1                          = atoi(skillrecipe.tab["CoolDownAdd1"].c_str());
    skillrecipe.CoolDownAdd2                          = atoi(skillrecipe.tab["CoolDownAdd2"].c_str());
    skillrecipe.CoolDownAdd3                          = atoi(skillrecipe.tab["CoolDownAdd3"].c_str());
    skillrecipe.DamageAddPercent                      = atoi(skillrecipe.tab["DamageAddPercent"].c_str());
    skillrecipe.hasScriptFile                         = !skillrecipe.tab["ScriptFile"].empty();
    // 将 Cooldown 存入缓存
    mapRecipe[std::make_tuple(RecipeID, RecipeLevel)] = std::move(skillrecipe);
}

void SkillRecipeManager::addScriptSkill(const SkillRecipe *skillrecipe, const Skill *skill) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断 SkillRecipe 是否存在.
    if (mapScriptSkill.find(std::make_tuple(skillrecipe, skill)) != mapScriptSkill.end()) {
        // 若 skillrecipe && skill 存在, 则直接返回
        return; // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }

    // 执行 GetSkillRecipeData
    Skill scriptskill;
    scriptskill.dwSkillID = skill->dwSkillID;
    scriptskill.dwLevel   = skill->dwLevel;
    std::string path      = "scripts/skill/" + skillrecipe->tab.at("ScriptFile");
    bool        res       = LuaFunc::analysis(
        LuaFunc::getGetSkillRecipeData(path)(
            scriptskill, skillrecipe->RecipeID, skillrecipe->RecipeLevel
        ),
        path,
        LuaFunc::Enum::GetSkillRecipeData
    );
    if (res) {
        // 成功执行, 将技能添加到 ScriptSkill 中
        mapScriptSkill[std::make_tuple(skillrecipe, skill)] = std::move(scriptskill);
    } else {
        CONSTEXPR_LOG_ERROR("LuaFunc::getGetSkillRecipeData(\"{}\") failed.", path);
    }
}
