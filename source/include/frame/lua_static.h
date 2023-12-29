#ifndef FRAME_LUA_STATIC_H_
#define FRAME_LUA_STATIC_H_

#include "frame/character/character.h"
#include "frame/global/skill.h"
#include <memory>
#include <sol/sol.hpp>
#include <string>
#include <vector>

namespace ns_frame {

std::shared_ptr<sol::state> luaInit();

extern const std::vector<std::string> luaFuncStaticToDynamic;

class LuaGlobalFunction {
public:
    LuaGlobalFunction() = delete;
    static bool       CheckInTongWar(ns_frame::Character *character);
    static bool       IsLangKeXingMap(int mapID);
    static bool       IsPlayer(int nCharacterID);
    static bool       IsTreasureBattleFieldMap(int mapID);
    static int        GetValueByBits(int nValue, int nBit, int c);
    static int        SetValueByBits(int nValue, int nBit, int c, int nNewBitValue);
    static int        GetDistanceSq(int pX, int pY, int pZ, int tX, int tY, int tZ);
    static int        Random(int min, int max);
    static void       AdditionalAttribute(ns_frame::Skill &skill);
    static void       Include(const std::string &filename);
    static void       ModityCDToUI(ns_frame::Character *character, int skillID, int c, int d);
    static void       RemoteCallToClient();
    static Character *GetPlayer(int nCharacterID);
    static Character *GetNpc(int nCharacterID);
};

} // namespace ns_frame

#endif // FRAME_LUA_STATIC_H_
