#ifndef MAIN_INTERFACE_INIT_H_
#define MAIN_INTERFACE_INIT_H_

#include <sol2/sol.hpp>

namespace ns_interface {

bool luaPreprocess(sol::state &lua);

// 初始化接口. 注意, 这是一个静态类, 无法创建实例.
class Init {
public:
    // 禁止创建类实例
    Init() = delete;

    /*
     * @brief 初始化接口
     * @return `luaPreprocess` 函数的指针, 将其作为参数传至 `gdi:InterfaceInstance:initLuaPreprocess` 即可完成接口的初始化.
     */
    static bool (*init())(sol::state &lua);
};

} // namespace ns_interface

#endif // MAIN_INTERFACE_INIT_H_