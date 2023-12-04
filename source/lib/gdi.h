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

namespace fs = std::filesystem;

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
    skillrepresent,
    buffrepresent,
    COUNT, // 用于计数, 请勿使用
};

/**
 * @brief 表查询数据类型
 * @see InterfaceInstance::tabSelect()
 */
using TabSelectType = std::vector<std::unordered_map<std::string, std::string>>;

// 接口实例. 每个并发的线程都应且仅应有一个 InterfaceInstance 实例.
class Interface {
public:
    Interface() = delete; // 禁止创建实例

    /**
     * @brief 初始化数据管理器
     * @param pJX3 JX3 目录
     * @param pDirUnpacked 未打包的数据目录
     * @return 是否成功
     */
    static bool initGameData(const fs::path &pJX3, const fs::path &pDirUnpacked);

    /**
     * @brief 初始化 lua
     * @param lua_init lua 初始化函数指针
     * @param staticFuncNeedConvert 需要从静态转换至动态的 lua 函数列表
     * @return 是否成功
     */
    static bool initLua(bool (*lua_init)(sol::state &lua), const std::vector<std::string> &staticFuncNeedConvert);

    /**
     * @brief 初始化 tab
     * @return 是否成功
     */
    static bool initTab(int tabCount);

    /**
     * @brief 执行游戏内 lua 脚本
     * @param filename 脚本路径
     * @param fileExists 文件是否存在. 如果文件不存在, 那么这个参数会被设置为 false.
     * @warning lua 的执行是线程本地的, 这意味着当前线程调用该函数不会影响其他线程!
     * @return 执行结果
     */
    static sol::protected_function_result luaExecuteFile(const std::string &filename, bool *fileExists);

    /**
     * @brief 获取 lua 函数
     * @param name 函数名
     * @warning lua 的执行是线程本地的, 不要让返回值被其他线程使用!
     * @return lua 函数
     */
    static sol::protected_function luaGetFunction(const std::string &name);

    /**
     * @brief 查询满足 key-value 的行
     * @param arg 是一个 `vector<unordered_map<string, string>>` 类型的变量, 用于传入和传出数据.
     * @return 满足条件的行数 (作为 arg 的前 n 行). 返回值应当等于 arg.size(), 否则代表查询过程中出现错误.
     * @note #
     * @note 当用于传入数据时:
     * @note - arg[i] 中的 key-value 对应查询条件, 且彼此之间为与的关系.
     * @note - 不同的 arg[i] 之间为或的关系. (通常情况下, arg.size() 应该为 1, 即不使用本条规则)
     * @note #
     * @note 当用于传出数据时, 每一条 arg[i] 都是一行满足查询条件的数据, 且对于任何 i, arg[i].size() 是一个定值 (field 的数量, 也即列数).
     */
    static int tabSelect(Tab tab, TabSelectType &arg);
};

} // namespace gdi

#endif // LIB_GDI_H