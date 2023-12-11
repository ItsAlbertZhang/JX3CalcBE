#ifndef FRAME_LUA_STATIC_H_
#define FRAME_LUA_STATIC_H_

#include "frame/character/character.h"
#include "frame/global/skill.h"
#include <sol2/sol.hpp>
#include <string>
#include <vector>

namespace ns_frame {

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
    static int GetDistanceSq(int pX, int pY, int pZ, int tX, int tY, int tZ);
};

} // namespace ns_frame

#endif // FRAME_LUA_STATIC_H_