#include "frame/lua_runtime.h"
#include "gdi.h"
#include "program/log.h"

using namespace ns_frame;
using namespace std;

int LuaFunc::getIndex(string &filename, bool reload) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    if (filenameMap.find(filename) == filenameMap.end()) {
        add(filename);
    } else if (reload) {
        gdi::luaExecuteFile(filename);
    }
    return filenameMap[filename];
}

const string &LuaFunc::getFilename(int idx) {
    return filenameList[idx];
}

bool LuaFunc::analysis(sol::protected_function_result res, std::string &filename, Enum func) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    if (!res.valid()) {
        sol::error err = res;
        LOG_ERROR("{} {}() failed: \n{}", filename, names[static_cast<int>(func)], err.what());
        return false;
    } else {
        LOG_INFO("{} {}() success.", filename, names[static_cast<int>(func)]);
        return true;
    }
}

bool LuaFunc::analysis(sol::protected_function_result res, int idx, Enum func) {
    // 传入的 idx 是先前通过 getIndex 获取的, 一定存在
    if (!res.valid()) {
        sol::error err = res;
        LOG_ERROR("{} {}() failed: \n{}", filenameList[idx], names[static_cast<int>(func)], err.what());
        return false;
    } else {
        LOG_INFO("{} {}() success.", filenameList[idx], names[static_cast<int>(func)]);
        return true;
    }
}

void LuaFunc::add(const std::string &filename) {
    // 由于 data 是线程本地的, 因此不用考虑线程安全问题
    filenameMap[filename] = static_cast<int>(filefuncList.size());
    filenameList.emplace_back(filename);
    std::vector<sol::protected_function> &funcs = filefuncList.emplace_back();
    if (gdi::luaExecuteFile(filename)) {
        LOG_INFO("luaExecuteFile success: {}.", filename);
        for (int i = 0; i < static_cast<int>(Enum::COUNT); i++) {
            /**
             * 实际上, 此处有可能取到上一个执行文件的函数. (例如, 当前文件没有 OnRemove 函数, 就会取到上一个文件的.)
             * 但这不会导致实际的问题: 游戏内逻辑不可能调用一个文件中不存在的函数. 因此, 其只会被保存, 而不会被调用.
             */
            funcs.emplace_back(gdi::luaGetFunction(names[i]));
        }
        return; // 执行成功, 直接返回
    }
    for (int i = 0; i < static_cast<int>(Enum::COUNT); i++) {
        funcs.emplace_back(gdi::luaGetFunction("FileNotExistEmptyFunction")); // 名称随意, 取一个空函数即可
    }
    LOG_ERROR("luaExecuteFile failed: {}.", filename);
}

void LuaFunc::clear() {
    filenameList.clear();
    filenameMap.clear();
    filefuncList.clear();
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