#ifndef FRAME_STATIC_LUA_H_
#define FRAME_STATIC_LUA_H_

#include "frame/character/character.h"
#include "frame/global/skill.h"
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
    static void AdditionalAttribute(ns_frame::Skill &skill);
    static bool IsLangKeXingMap(int mapID);
    static void ModityCDToUI(ns_frame::Character *character, int skillID, int c, int d);
    static bool CheckInTongWar(ns_frame::Character *character);
    static bool IsTreasureBattleFieldMap(int mapID);
    static int GetValueByBits(int nValue, int nBit, int c);
    static int SetValueByBits(int nValue, int nBit, int c, int nNewBitValue);
    static void RemoteCallToClient();
};

} // namespace ns_framestatic

#endif // FRAME_STATIC_LUA_H_