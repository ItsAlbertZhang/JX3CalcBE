#ifndef FRAME_LUA_STATIC_H_
#define FRAME_LUA_STATIC_H_

#include "frame/character/character.h"
#include <sol/sol.hpp>
#include <string>

namespace ns_frame {

sol::state *luaInit();

extern const char *const luaFuncList[];
extern const size_t      luaFuncListSize;

class LuaGlobalFunction {
public:
    LuaGlobalFunction() = delete;
    static bool        CheckInTongWar(ns_frame::Character *character);
    static bool        IsLangKeXingMap(int mapID);
    static bool        IsClient();
    static bool        IsPlayer(int nCharacterID);
    static bool        IsTreasureBattleFieldMap(int mapID);
    static int         GetDistanceSq(int pX, int pY, int pZ, int tX, int tY, int tZ);
    static std::string GetEditorString(int a, int b);
    static int         GetValueByBits(int nValue, int nBit, int c);
    static int         SetValueByBits(int nValue, int nBit, int c, int nNewBitValue);
    static int         Random(int min, int max);
    static void        Include(const std::string &filename);
    static void        ModityCDToUI(ns_frame::Character *character, int skillID, int c, int d);
    static void        RemoteCallToClient();
    static Character  *GetPlayer(int nCharacterID);
    static Character  *GetNpc(int nCharacterID);
};

} // namespace ns_frame

#endif // FRAME_LUA_STATIC_H_
