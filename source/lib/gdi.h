#ifndef LIB_GDI_H
#define LIB_GDI_H

#include <filesystem>
#include <sol2/sol.hpp>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief 此文件为 gdi 库允许被调用的接口.
 * @warning 请勿随意更改本文件内容!
 */

namespace gdi {

/**
 * @brief 表枚举
 * @warning 请勿更改本枚举类! 如需添加表, 请联系作者同步更改 gdi 库.
 */
enum class Tab {
    skills,
    buff,
    cooldown,
    skillrecipe,
    skillevent,
    ui_skill,
    ui_buff,
    COUNT, // 用于计数, 请勿使用
};

/**
 * @brief 初始化 gdi 库
 * @param dataPathJX3           剑网3 (JX3) 目录. 该项可以为空, 但不可与 dataPathUnpacked 同时为空.
 * @param dataPathUnpacked      未打包的数据目录. 该项可以为空, 但不可与 dataPathJX3 同时为空.
 * @param luaInit               lua 初始化函数指针, 其应当在正确运行时返回 true.
 * @param luaFuncNeedConvert    需要从静态转换至动态的 lua 函数列表
 * @param tabCount              表的数量
 * @warning 这个函数只需要在主线程中调用一次!
 */
using luaInit_t = bool (*)(sol::state &);
bool init(
    const std::filesystem::path    &dataPathJX3,
    const std::filesystem::path    &dataPathUnpacked,
    luaInit_t                       luaInit,
    const std::vector<std::string> &luaFuncNeedConvert,
    int                             tabCount
);

/**
 * @brief 执行游戏内 lua 脚本
 * @param filename 脚本路径
 * @warning lua 的执行是线程本地的, 这意味着当前线程调用该函数不会影响其他线程!
 * @note 如果执行失败且定义了 DEBUG 宏变量, 会在 cerr 输出错误信息.
 */
bool luaExecuteFile(const std::string &filename);

/**
 * @brief 获取 lua 函数
 * @param name 函数名
 * @warning lua 的执行是线程本地的, 不要让返回值被其他线程使用!
 * @return lua 函数
 */
sol::protected_function luaGetFunction(const std::string &name);

/**
 * @brief 表查询数据类型
 * @see InterfaceInstance::tabSelect()
 */
using select_t = std::vector<std::unordered_map<std::string, std::string>>;

/**
 * @brief 查询满足 key-value 的行
 * @param arg 是一个 `vector<unordered_map<string, string>>` 类型的变量, 用于传入和传出数据.
 * @note #
 * @note 当用于传入数据时:
 * @note - arg[i] 中的 key-value 对应查询条件, 且彼此之间为与的关系.
 * @note - 不同的 arg[i] 之间为或的关系. (通常情况下, arg.size() 应该为 1, 即不使用本条规则)
 * @note #
 * @note 当用于传出数据时, 每一条 arg[i] 都是一行满足查询条件的数据, 且对于任何 i, arg[i].size() 是一个定值 (field 的数量, 也即列数).
 */
void tabSelect(Tab tab, select_t &arg);

} // namespace gdi

#endif // LIB_GDI_H
