#include "frame/lua_runtime.h"
#include "frame/statics/lua_blacklist_files.h"
#include "gdi.h"
#include "plugin/log.h"

#define UNREFERENCED_PARAMETER(P) (P)

using namespace ns_frame;
using namespace std;

sol::state *LuaFunc::getLua() {
    return &lua;
}

int LuaFunc::getIndex(string &filename, bool reload) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    if (filenameMap.find(filename) == filenameMap.end()) {
        add(filename);
    } else if (reload) {
        gdi::luaExecuteFile(filename.c_str());
    }
    return filenameMap[filename];
}

const string &LuaFunc::getFilename(int idx) {
    return filenameList[idx];
}

bool LuaFunc::analysis(sol::protected_function_result res, std::string &filename, Enum func) {
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

bool LuaFunc::analysis(sol::protected_function_result res, int idx, Enum func) {
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

void LuaFunc::add(const std::string &filename) {
    // 由于 data 是线程本地的, 因此不用考虑线程安全问题
    filenameMap[filename] = static_cast<int>(filefuncList.size());
    filenameList.emplace_back(filename);
    std::vector<sol::protected_function> &funcs = filefuncList.emplace_back();
    if (!staticsLuaBlacklistFiles.contains(filename) && 0 == gdi::luaExecuteFile(filename.c_str())) {
        CONSTEXPR_LOG_INFO("luaExecuteFile success: {}.", filename);
        for (int i = 0; i < static_cast<int>(Enum::COUNT); i++) {
            /**
             * 实际上, 此处有可能取到上一个执行文件的函数. (例如, 当前文件没有 OnRemove 函数, 就会取到上一个文件的.)
             * 但这不会导致实际的问题: 游戏内逻辑不可能调用一个文件中不存在的函数. 因此, 其只会被保存, 而不会被调用.
             */
            funcs.emplace_back(gdi::luaGetFunction(names[i].c_str()));
        }
        return; // 执行成功, 直接返回
    }
    for (int i = 0; i < static_cast<int>(Enum::COUNT); i++) {
        funcs.emplace_back(gdi::luaGetFunction("FileNotExistEmptyFunction")); // 名称随意, 取一个空函数即可
    }
    CONSTEXPR_LOG_ERROR("luaExecuteFile failed: {}.", filename);
}

sol::protected_function LuaFunc::getGetSkillLevelData(string &filename) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    int idx = getIndex(filename, true); // 执行 getIndex 后, 一定存在
    return filefuncList[idx][static_cast<int>(Enum::GetSkillLevelData)];
}

sol::protected_function LuaFunc::getGetSkillRecipeData(string &filename) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    int idx = getIndex(filename, true); // 执行 getIndex 后, 一定存在
    return filefuncList[idx][static_cast<int>(Enum::GetSkillRecipeData)];
}

sol::protected_function LuaFunc::getApply(string &filename) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    int idx = getIndex(filename); // 执行 getIndex 后, 一定存在
    return filefuncList[idx][static_cast<int>(Enum::Apply)];
}

sol::protected_function LuaFunc::getUnApply(string &filename) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    int idx = getIndex(filename); // 执行 getIndex 后, 一定存在
    return filefuncList[idx][static_cast<int>(Enum::UnApply)];
}

sol::protected_function LuaFunc::getOnRemove(string &filename) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    int idx = getIndex(filename); // 执行 getIndex 后, 一定存在
    return filefuncList[idx][static_cast<int>(Enum::OnRemove)];
}

sol::protected_function LuaFunc::getOnTimer(int idx) {
    // 传入的 idx 是先前通过 getIndex 获取的, 一定存在
    return filefuncList[idx][static_cast<int>(Enum::OnTimer)];
}

void LuaFunc::include(const std::string &filename) {
    if (includedFiles.contains(filename)) {
        return;
    }
    includedFiles.insert(filename);
    if (!staticsLuaBlacklistFiles.contains(filename))
        gdi::luaExecuteFile(filename.c_str());
}
