#include "frame/lua/interface.h"
#include "frame/lua/statics.h"
#include "gdi.h"
#include "plugin/log.h"

#define UNREFERENCED_PARAMETER(P) (P)

using namespace jx3calc;
using namespace frame;

static const std::string names[] = {
    "GetSkillLevelData",
    "GetSkillRecipeData",
    "Apply",
    "UnApply",
    "OnRemove",
    "OnTimer",
};
static thread_local sol::state                                        luaState;
static thread_local std::vector<std::string>                          filenameList;
static thread_local std::unordered_map<std::string, int>              filenameMap;
static thread_local std::vector<std::vector<sol::protected_function>> filefuncList;
static thread_local std::unordered_set<std::string>                   includedFiles;

static const std::string &getFilename(int idx) {
    return filenameList[idx];
}

static void add(const std::string &filename) {
    // 由于 data 是线程本地的, 因此不用考虑线程安全问题
    filenameMap[filename] = static_cast<int>(filefuncList.size());
    filenameList.emplace_back(filename);
    std::vector<sol::protected_function> &funcs = filefuncList.emplace_back();
    if (!lua::statics::LuaBlacklistFiles.contains(filename) && 0 == gdi::luaExecuteFile(filename.c_str())) {
        CONSTEXPR_LOG_INFO("luaExecuteFile success: {}.", filename);
        for (int i = 0; i < static_cast<int>(lua::interface::FuncType::COUNT); i++) {
            /**
             * 实际上, 此处有可能取到上一个执行文件的函数. (例如, 当前文件没有 OnRemove 函数, 就会取到上一个文件的.)
             * 但这不会导致实际的问题: 游戏内逻辑不可能调用一个文件中不存在的函数. 因此, 其只会被保存, 而不会被调用.
             */
            funcs.emplace_back(gdi::luaGetFunction(names[i].c_str()));
        }
        return; // 执行成功, 直接返回
    }
    for (int i = 0; i < static_cast<int>(lua::interface::FuncType::COUNT); i++) {
        funcs.emplace_back(gdi::luaGetFunction("FileNotExistEmptyFunction")); // 名称随意, 取一个空函数即可
    }
    CONSTEXPR_LOG_ERROR("luaExecuteFile failed: {}.", filename);
}

sol::state *lua::interface::getLuaState() {
    return &luaState;
}

int lua::interface::getIndex(std::string &filename, bool reload) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    if (filenameMap.find(filename) == filenameMap.end()) {
        add(filename);
    } else if (reload) {
        gdi::luaExecuteFile(filename.c_str());
    }
    return filenameMap[filename];
}

bool lua::interface::analysis(sol::protected_function_result res, std::string &filename, FuncType func) {
    UNREFERENCED_PARAMETER(filename); // unreferenced in release mode
    UNREFERENCED_PARAMETER(func);     // unreferenced in release mode
    std::replace(filename.begin(), filename.end(), '\\', '/');
    if (!res.valid()) {
        sol::error err = res;
        CONSTEXPR_LOG_ERROR("{} {}() failed: \n{}", filename, names[static_cast<int>(func)], err.what());
        return false;
    } else {
        CONSTEXPR_LOG_INFO("{} {}() success.", filename, names[static_cast<int>(func)]);
        return true;
    }
}

bool lua::interface::analysis(sol::protected_function_result res, int idx, FuncType func) {
    UNREFERENCED_PARAMETER(idx);  // unreferenced in release mode
    UNREFERENCED_PARAMETER(func); // unreferenced in release mode
    // 传入的 idx 是先前通过 getIndex 获取的, 一定存在
    if (!res.valid()) {
        sol::error err = res;
        CONSTEXPR_LOG_ERROR("{} {}() failed: \n{}", filenameList[idx], names[static_cast<int>(func)], err.what());
        return false;
    } else {
        CONSTEXPR_LOG_INFO("{} {}() success.", filenameList[idx], names[static_cast<int>(func)]);
        return true;
    }
}

sol::protected_function lua::interface::getGetSkillLevelData(std::string &filename) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    int idx = getIndex(filename, true); // 执行 getIndex 后, 一定存在
    return filefuncList[idx][static_cast<int>(FuncType::GetSkillLevelData)];
}

sol::protected_function lua::interface::getGetSkillRecipeData(std::string &filename) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    int idx = getIndex(filename, true); // 执行 getIndex 后, 一定存在
    return filefuncList[idx][static_cast<int>(FuncType::GetSkillRecipeData)];
}

sol::protected_function lua::interface::getApply(std::string &filename) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    int idx = getIndex(filename); // 执行 getIndex 后, 一定存在
    return filefuncList[idx][static_cast<int>(FuncType::Apply)];
}

sol::protected_function lua::interface::getUnApply(std::string &filename) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    int idx = getIndex(filename); // 执行 getIndex 后, 一定存在
    return filefuncList[idx][static_cast<int>(FuncType::UnApply)];
}

sol::protected_function lua::interface::getOnRemove(std::string &filename) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    int idx = getIndex(filename); // 执行 getIndex 后, 一定存在
    return filefuncList[idx][static_cast<int>(FuncType::OnRemove)];
}

sol::protected_function lua::interface::getOnTimer(int idx) {
    // 传入的 idx 是先前通过 getIndex 获取的, 一定存在
    return filefuncList[idx][static_cast<int>(FuncType::OnTimer)];
}

void lua::interface::include(const std::string &filename) {
    if (includedFiles.contains(filename)) {
        return;
    }
    includedFiles.insert(filename);
    if (!lua::statics::LuaBlacklistFiles.contains(filename))
        gdi::luaExecuteFile(filename.c_str());
}
