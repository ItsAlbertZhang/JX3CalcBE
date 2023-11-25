#ifndef FRAME_STATIC_LUA_H_
#define FRAME_STATIC_LUA_H_

#include "frame/character/character.h"
#include <sol2/sol.hpp>
#include <string>
#include <vector>

namespace ns_framestatic {

bool luaInit(sol::state &lua);

extern const std::vector<std::string> luaFuncStaticToDynamic;

class LuaGlobalFunction {
public:
    LuaGlobalFunction() = delete;
    static void Include(const std::string &filename);
    static ns_frame::Character *GetPlayer(int nCharacterID);
    static ns_frame::Character *GetNpc(int nCharacterID);
    static bool IsPlayer(int nCharacterID);
};

} // namespace ns_framestatic

#endif // FRAME_STATIC_LUA_H_