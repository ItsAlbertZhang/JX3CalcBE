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
 * @brief 接口实例资源
 * @note 具体实现此处不可见
 */
class InterfaceInstanceResource;

/**
 * @brief 表枚举
 * @warning 请勿更改本枚举类! 如需添加表, 请联系作者同步更改 gdi 库.
 */
enum class Tab {
    skills,
    buff,
    COUNT, // 用于计数, 请勿使用
};

/**
 * @brief 表索引
 * @see InterfaceInstance::tabAddIndex()
 * @note 此处的索引字段会在表创建后被自动添加.
 * @note `tab_index.size()` 应当在数值上等于 `Tab::COUNT`.
 */
const std::vector<std::vector<std::string>> tab_index = {
    {
        "SkillID",
    }, // skills.tab
    {
        "ID",
        "Level",
    }, // buff.tab
};

/**
 * @brief 表查询数据类型
 * @see InterfaceInstance::tabSelect()
 */
using TabSelectType = std::vector<std::unordered_map<std::string, std::string>>;

// 接口实例. 每个并发的线程都应且仅应有一个 InterfaceInstance 实例.
class InterfaceInstance {
public:
    InterfaceInstance();  // 构造函数
    ~InterfaceInstance(); // 析构函数

    /**
     * @brief 初始化数据管理器
     * @param pJX3 JX3 目录
     * @param pDirUnpacked 未打包的数据目录
     * @return 是否成功
     * @warning 请确保在创建类的实例前调用此函数.
     */
    static bool initGameData(const fs::path &pJX3, const fs::path &pDirUnpacked);

    /**
     * @brief 初始化 lua 脚本预处理函数
     * @param func 预处理函数指针, 函数原型应为 bool (*)(sol::state &lua)
     * @return 是否成功
     * @warning 请确保在创建类的实例前调用此函数.
     */
    static bool initLuaPreprocess(bool (*func)(sol::state &lua));

    /**
     * @brief 初始化当前线程的接口实例指针
     */
    static bool initPtrInterface(InterfaceInstance *&ptrInterface);

    /**
     * @brief 执行游戏内 lua 脚本
     * @param filename 脚本路径
     * @return 执行结果
     */
    sol::protected_function_result luaExecuteFile(const std::string &filename);

    /**
     * @brief 获取 lua 函数
     * @param name 函数名
     * @return lua 函数
     */
    sol::protected_function luaGetFunction(const std::string &name);

    /**
     * @brief 添加索引
     * @param keyVector 索引字段名的 `vector`. 靠前的字段优先级更高.
     * @note 为表添加索引, 以提高查询效率. 在上文 `tab_index` 中定义的索引会被默认添加. 需要注意:
     * @note - 索引字段必须具有顺序结构 (即在原始数据中, 该字段必须是按顺序排列的).
     * @note - 索引字段不必唯一 (即在原始数据中, 该字段可以有重复值).
     * @note - 若 `keyVector.size()` > 1, 则代表使用组合索引. 组合索引具有优先级, 靠前的索引优先级更高.
     * @note - 如果使用组合索引, 最高优先级的索引必须对于整表有序, 次优先级的索引必须对于更高优先级的索引有序.
     * @warning 本函数仅检查索引字段是否存在, 不检查索引字段是否有序. 如果索引字段是无序的, 则可能导致查询结果不正确.
     */
    bool tabAddIndex(Tab tab, const std::vector<std::string> &keyVector);

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
    int tabSelect(Tab tab, TabSelectType &arg);

private:
    InterfaceInstanceResource *resource = nullptr;
    static thread_local bool initedGameData;
    static thread_local bool initedLuaPreprocess;
};

extern thread_local InterfaceInstance *ptrInterface; // 当前线程的接口实例指针

} // namespace gdi

#endif // LIB_GDI_H