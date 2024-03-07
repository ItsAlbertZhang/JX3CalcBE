#ifndef FRAME_CUSTOM_BASE_H_
#define FRAME_CUSTOM_BASE_H_

#include <string>
#include <unordered_map>
namespace ns_frame {

enum class enumCustom {
    none,
    lua,
    // jx3,
};

inline std::unordered_map<std::string, enumCustom> refCustom{
    {"使用内置循环", enumCustom::none},
    {"使用lua脚本",    enumCustom::lua },
 // {"使用游戏内宏", enumCustom::jx3 },
};

} // namespace ns_frame

#endif // FRAME_CUSTOM_BASE_H_
