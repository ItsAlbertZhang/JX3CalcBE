/**
 * @file gdi.h
 * @author Albert Zhang
 * @brief 该文件定义了 gdi 库的接口.
 * @note gdi 库仅发布 Release 版本. 由于 std::string 与 STL 容器在 Debug / Release 模式下的实现不同, 因此无法使用.
 * @note 因此, 除 sol 库对象外, 本库的接口全部使用 C 风格的字符串和数组.
 * @warning 请勿随意更改本文件内容.
 */

#ifndef LIB_GDI_H
#define LIB_GDI_H

#include <cstring>
#include <sol/sol.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#define UNREFERENCED_PARAMETER(P) (P)

namespace gdi {

/**
 * @brief 初始化 data.
 * @param jx3Dir 剑网3 (JX3) 目录. 该项可以为空, 但不可与 unpackDir 同时为空.
 * @param unpackDir 未打包的数据目录. 该项可以为空, 但不可与 jx3Dir 同时为空.
 * @warning 传入的字符串中如果包含中文, 应当为 utf-8 编码.
 * @return 0 表示成功, -1 表示失败.
 */
int dataInit(const char *jx3Dir, const char *unpackDir);

/**
 * @brief 初始化 lua.
 * @param initLua lua 初始化函数.
 * @param luaFuncList lua 函数列表.
 * @param luaFuncListSize lua 函数列表大小.
 * @return 0 表示成功, -1 表示失败.
 * @details
 * `initLua` 函数应当返回一个 sol::state 对象的指针. 需要确保这个对象是线程安全的.
 * gdi 库会保存这个指针直至线程结束, 并使用它作为资源完成下面的 `luaExecuteFile`, `luaGetFunction` 操作.
 * 在一个线程中传入 `initLua` 函数后, 你就可以在不同的线程中直接调用操作函数了. gdi 库会在每个线程中自动调用 `initLua` 函数进行初始化.
 * 注意! 确保在 `initLua` 函数在不同的线程中返回指向不同的对象的指针, 否则可能会出现严重的数据竞争问题.
 */
int luaInit(sol::state *(*initLua)(), const char *const luaFuncList[], size_t luaFuncListSize);

/**
 * @brief 执行 lua 文件.
 * @param path 文件路径.
 * @warning 传入的字符串中如果包含中文, 应当为 utf-8 编码.
 * @note 如果 path 是从 tab 获取的, 那么它天生就是 utf-8 编码的.
 * @return int 0 表示成功, -1 表示失败.
 */
int luaExecuteFile(const char *path);

/**
 * @brief 获取 lua 函数.
 * @param name 函数名.
 * @warning 注意在 luaInit 函数的 details 中强调的线程安全问题, 不要让返回值被其他线程使用!
 * @return lua 函数.
 */
sol::protected_function luaGetFunction(const char *name);

/**
 * @brief 表枚举
 * @warning 请勿更改本枚举类! 如需更改表, 必须同步更改 gdi 库.
 */
enum class Tab {
    skills,
    buff,
    cooldown,
    skillrecipe,
    skillevent,
    custom_armor,
    custom_trinket,
    custom_weapon,
    ui_skill,
    ui_buff,
    COUNT, // 用于计数, 请勿使用
};

/**
 * @brief 数据传递结构. 用于传递查询条件和结果.
 *
 * @note ### 当作为查询条件时:
 * @note `unordered_map<string, string>` 中的不同 key - value 彼此之间为 **与** 的关系.
 * @note `vector<unordered_map>` 中的不同 `unordered_map` 之间为 **或** 的关系.
 * @note ### 当作为查询结果时:
 * @note 每一个 unordered_map 都是一条满足条件的数据, 且 `vector<unordered_map>` 中每个 `unordered_map` 的 `.size()` 相等 (等于 tab 的字段数/列数).
 */
using select_t = std::vector<std::unordered_map<std::string, std::string>>;

// select_t 至 char * 的序列化
inline char *serialize(const select_t &data, char *buffer, size_t size) {
    UNREFERENCED_PARAMETER(size); // 偷懒, 不判断 size 是否足够, buff.tab 都没 16M
    size_t offset     = 0;
    // 序列化 vector 的大小
    size_t vecotrSize = data.size();
    std::memcpy(buffer + offset, &vecotrSize, sizeof(size_t));
    offset += sizeof(size_t);
    // 序列化 vector 中的每一个 map
    for (const auto &map : data) {
        // 序列化 map 的大小
        size_t mapSize = map.size();
        std::memcpy(buffer + offset, &mapSize, sizeof(size_t));
        offset += sizeof(size_t);
        // 序列化 map 中的每一个 key-value 对
        for (const auto &pair : map) {
            // 序列化 key 的大小和内容
            size_t keySize = pair.first.length();
            std::memcpy(buffer + offset, &keySize, sizeof(size_t));
            offset += sizeof(size_t);
            std::memcpy(buffer + offset, pair.first.c_str(), keySize);
            offset += keySize;
            // 序列化 value 的大小和内容
            size_t valueSize = pair.second.length();
            std::memcpy(buffer + offset, &valueSize, sizeof(size_t));
            offset += sizeof(size_t);
            std::memcpy(buffer + offset, pair.second.c_str(), valueSize);
            offset += valueSize;
        }
    }
    return buffer; // 返回 buffer, 方便调用
}

// char * 至 select_t 的反序列化
inline select_t deserialize(const char *buffer, size_t size) {
    UNREFERENCED_PARAMETER(size);
    select_t data;
    size_t   offset = 0;

    // 反序列化 vector 大小
    size_t vectorSize;
    std::memcpy(&vectorSize, buffer + offset, sizeof(size_t));
    offset += sizeof(size_t);
    data.reserve(vectorSize);

    // 反序列化 vector 中的每一个 map
    for (size_t vectorIdx = 0; vectorIdx < vectorSize; vectorIdx++) {
        std::unordered_map<std::string, std::string> map;

        // 反序列化 map 大小
        size_t mapSize;
        std::memcpy(&mapSize, buffer + offset, sizeof(size_t));
        offset += sizeof(size_t);
        map.reserve(mapSize);

        // 反序列化 map 中的每一个 key-value 对
        for (size_t mapIdx = 0; mapIdx < mapSize; mapIdx++) {
            // 反序列化 key 的大小和内容
            size_t keySize;
            std::memcpy(&keySize, buffer + offset, sizeof(size_t));
            offset += sizeof(size_t);
            std::string key(buffer + offset, keySize);
            offset += keySize;
            // 反序列化 value 的大小和内容
            size_t valueSize;
            std::memcpy(&valueSize, buffer + offset, sizeof(size_t));
            offset += sizeof(size_t);
            std::string value(buffer + offset, valueSize);
            offset += valueSize;

            map.emplace(std::move(key), std::move(value));
        }
        data.emplace_back(std::move(map));
    }
    return data;
}

void tabQuery(int tabIdx, char *data, size_t dataSize); // 实际接口

/**
 * @brief 查询满足 key-value 的行
 * @param arg 是一个 `select_t` 类型的变量, 用于传入和传出数据. 详情请查看其定义.
 * @warning 对于相同的表而言, tabSelect 不是线程安全的.
 * 在每个线程中维护同一份数据从设计角度上来说是不必要的. 请自行确保进入同一份表时的线程安全.
 * @warning 这个函数不会被 gdi 库定义, 实现见 gdi.cpp .
 */
void tabSelect(Tab tab, select_t &arg);

} // namespace gdi

#endif // LIB_GDI_H
