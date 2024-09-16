#include "frame/global/skillrecipe.h"
#include "frame/lua/interface.h"
#include "gdi.h"
#include "plugin/log.h"
#include <tuple>

using namespace jx3calc;
using namespace frame;

const SkillRecipe &SkillRecipeManager::getRecipe(int RecipeID, int RecipeLevel) {
    if (!mapRecipe.contains(std::make_tuple(RecipeID, RecipeLevel)))
        // 若不存在, 则添加并返回
        return add(RecipeID, RecipeLevel);
    else
        return mapRecipe.at(std::make_tuple(RecipeID, RecipeLevel));
}

const Skill *SkillRecipeManager::getScriptSkill(const SkillRecipe *skillrecipe, const Skill *skill) {
    if (!skillrecipe->hasScriptFile || skillrecipe == nullptr || skill == nullptr)
        return nullptr;
    if (!mapScriptSkill.contains(std::make_tuple(skillrecipe, skill)))
        // 若不存在, 则添加并返回
        return addScriptSkill(skillrecipe, skill);
    else
        return &mapScriptSkill.at(std::make_tuple(skillrecipe, skill));
}

const SkillRecipe &SkillRecipeManager::add(int RecipeID, int RecipeLevel) {
    std::lock_guard<std::mutex> lock(mutex); // 加锁
    std::tuple<int, int>        key = std::make_tuple(RecipeID, RecipeLevel);
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断 SkillRecipe 是否存在.
    if (mapRecipe.contains(key)) {
        // 若 Recipe ID && Recipe 等级存在, 则直接返回
        return mapRecipe.at(key); // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
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
    if (arg.size() == 0) {
        CONSTEXPR_LOG_ERROR("SkillRecipeManager::add: Recipe ID {} 不存在.", RecipeID);
        mapRecipe[key] = std::move(skillrecipe);
        return mapRecipe.at(key);
    }
    skillrecipe.tab              = std::move(arg[0]);
    // 初始化数据. std::stoi() 用于确定字段存在的情况. 若该字段可能为空, 必须使用 atoi().
    skillrecipe.SkillRecipeType  = atoi(skillrecipe.tab["SkillRecipeType"].c_str());
    skillrecipe.SkillID          = atoi(skillrecipe.tab["SkillID"].c_str());
    skillrecipe.CoolDownAdd1     = atoi(skillrecipe.tab["CoolDownAdd1"].c_str());
    skillrecipe.CoolDownAdd2     = atoi(skillrecipe.tab["CoolDownAdd2"].c_str());
    skillrecipe.CoolDownAdd3     = atoi(skillrecipe.tab["CoolDownAdd3"].c_str());
    skillrecipe.DamageAddPercent = atoi(skillrecipe.tab["DamageAddPercent"].c_str());
    skillrecipe.hasScriptFile    = !skillrecipe.tab["ScriptFile"].empty();

    // 将 Cooldown 存入缓存
    mapRecipe[key] = std::move(skillrecipe);
    return mapRecipe.at(key);
}

const Skill *SkillRecipeManager::addScriptSkill(const SkillRecipe *skillrecipe, const Skill *skill) {
    std::lock_guard<std::mutex>                    lock(mutex); // 加锁
    std::tuple<const SkillRecipe *, const Skill *> key = std::make_tuple(skillrecipe, skill);
    // 可能有多个线程同时进入了 add 函数. 因此, 需要在加锁后再次判断 SkillRecipe 是否存在.
    if (mapScriptSkill.contains(key)) {
        // 若 skillrecipe && skill 存在, 则直接返回
        return &mapScriptSkill.at(std::make_tuple(skillrecipe, skill)); // 返回时, 会自动调用 lock 的析构函数, 从而释放锁
    }

    // 执行 GetSkillRecipeData
    Skill scriptskill;
    scriptskill.dwSkillID = skill->dwSkillID;
    scriptskill.dwLevel   = skill->dwLevel;
    std::string path      = "scripts/skill/" + skillrecipe->tab.at("ScriptFile");
    bool        res       = lua::interface::analysis(
        lua::interface::getGetSkillRecipeData(path)(
            scriptskill, skillrecipe->RecipeID, skillrecipe->RecipeLevel
        ),
        path,
        lua::interface::FuncType::GetSkillRecipeData
    );
    if (!res) {
        CONSTEXPR_LOG_ERROR("LuaFunc::getGetSkillRecipeData(\"{}\") failed.", path);
    }

    // 将 ScriptSkill 存入缓存
    mapScriptSkill[key] = std::move(scriptskill);
    return &mapScriptSkill.at(key);
}
