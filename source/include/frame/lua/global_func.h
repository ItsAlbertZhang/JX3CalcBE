#pragma once

#include "frame/character/character.h"
#include <string>

namespace jx3calc {
namespace frame {
namespace lua {

namespace gfunc {

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

} // namespace gfunc

} // namespace lua
} // namespace frame
} // namespace jx3calc
