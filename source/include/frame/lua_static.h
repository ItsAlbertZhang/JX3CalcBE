#ifndef FRAME_LUA_STATIC_H_
#define FRAME_LUA_STATIC_H_

#include "frame/character/character.h"
#include <sol/sol.hpp>
#include <string>

namespace jx3calc {
namespace frame {

sol::state *luaInit();

extern const char *const luaFuncList[];
extern const size_t      luaFuncListSize;

namespace LuaGlobalFunc {
bool        CheckInTongWar(frame::Character *character);
bool        IsLangKeXingMap(int mapID);
bool        IsClient();
bool        IsPlayer(int nCharacterID);
bool        IsTreasureBattleFieldMap(int mapID);
int         GetDistanceSq(int pX, int pY, int pZ, int tX, int tY, int tZ);
std::string GetEditorString(int a, int b);
int         GetValueByBits(int nValue, int nBit, int c);
int         SetValueByBits(int nValue, int nBit, int c, int nNewBitValue);
int         Random(int min, int max);
void        Include(const std::string &filename);
void        ModityCDToUI(frame::Character *character, int skillID, int c, int d);
void        RemoteCallToClient();
Character  *GetPlayer(int nCharacterID);
Character  *GetNpc(int nCharacterID);
}; // namespace LuaGlobalFunc

} // namespace frame
} // namespace jx3calc

#endif // FRAME_LUA_STATIC_H_
