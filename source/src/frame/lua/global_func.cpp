#include "frame/character/character.h"
#include "frame/lua/global_func.h"
#include "frame/lua/interface.h"
#include "plugin/log.h"
#include <random>
#include <sol/sol.hpp>
#include <string>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace jx3calc;
using namespace frame;

void lua::gfunc::Include(const std::string &filename) {
    lua::interface::include(filename);
    return;
}

Character *lua::gfunc::GetPlayer(int nCharacterID) {
    return Character::characterGet(nCharacterID);
}

Character *lua::gfunc::GetNpc(int nCharacterID) {
    return Character::characterGet(nCharacterID);
}

bool lua::gfunc::IsPlayer(int nCharacterID) {
    return Character::characterGet(nCharacterID)->isPlayer;
}

bool lua::gfunc::IsLangKeXingMap(int mapID) {
    UNREFERENCED_PARAMETER(mapID);
    return false;
}

void lua::gfunc::ModityCDToUI(frame::Character *character, int skillID, int c, int d) {
    UNREFERENCED_PARAMETER(character);
    UNREFERENCED_PARAMETER(skillID);
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(d);
    return;
}

bool lua::gfunc::CheckInTongWar(frame::Character *character) {
    UNREFERENCED_PARAMETER(character);
    return false;
}

bool lua::gfunc::IsTreasureBattleFieldMap(int mapID) {
    UNREFERENCED_PARAMETER(mapID);
    return false;
}

/* ```lua
-- 函数名	: CustomFunction.GetValueByBit
-- 函数描述	: 获得数字的某一比特位值
-- 参数列表	:  nValue：值；nBit：取值范围[0,31]
-- 返回值	: 1 or 0
-- 备注		: CustomFunction.GetValueByBit(10, 31)，表示获取数字“10”的第31个Bit位的值。
function CustomFunction.GetValueByBit(nValue, nBit)
    if nBit > 31 or nBit < 0 then
        print(">>>>>>>CustomFunction.GetValueByBit Arg ERROR!!!!!BitIndex error")
        --return nValue
    end
    return GetValueByBits(nValue, nBit, 1)
    --return math.floor(nValue / 2 ^ nBit) % 2
end
``` */
int lua::gfunc::GetValueByBits(int nValue, int nBit, int c) {
    UNREFERENCED_PARAMETER(c);
    if (nBit > 31 || nBit < 0) {
        CONSTEXPR_LOG_ERROR(">>>>>>>CustomFunction.GetValueByBit Arg ERROR!!!!!BitIndex error{}", "");
    }
    return (nValue >> nBit) & 1;
}

/* ```lua
-- 函数名	: CustomFunction.SetValueByBit
-- 函数描述	: 将某个值的某一比特位值设置为0或者1
-- 参数列表	:  nValue：值；nBit：取值范围[0,31]；nNewBit：只能为0或者1
-- 返回值	: 设置完bit之后的新值
-- 备注		: CustomFunction.SetValueByBit(10,31,1)，表示将数字“10”的第31个Bit位的值置为1
function CustomFunction.SetValueByBit(nValue, nBit, nNewBitValue)
    if nNewBitValue > 1 or nNewBitValue < 0 then
        print(">>>>>>>CustomFunction.SetValueByBit Arg ERROR!!!!!nNewBit Must be 0 or 1,")
        return nValue
    end
    if nBit > 31 or nBit < 0 then
        print(">>>>>>>CustomFunction.SetValueByBit Arg ERROR!!!!!BitIndex error")
        return nValue
    end
    return SetValueByBits(nValue, nBit, 1, nNewBitValue)
    --如果要设置的新值和旧值一样，那么值不变，直接返回
    --if math.floor(nValue / 2 ^ nBit) % 2 == nNewBitValue then
        --return nValue
    --end
    --如果参数不正确那么
    --if nNewBitValue > 1 or nNewBitValue < 0 or nBit > 31 or nBit < 0 then
        --print(">>>>>>>CustomFunction.SetValueByBit Arg ERROR!!!!!nBit=[] nNewBit Must be 0 or 1,")
        --return nValue
    --end
    --0设成1加值，1变成0减值
    --print("SetValueByBit="..nNewBitValue)
    --if nNewBitValue == 1 then
        --return nValue + 2 ^ nBit
    --else
        --return nValue - 2 ^ nBit
    --end
end
``` */
int lua::gfunc::SetValueByBits(int nValue, int nBit, int c, int nNewBitValue) {
    UNREFERENCED_PARAMETER(c);
    if (nNewBitValue > 1 || nNewBitValue < 0) {
        CONSTEXPR_LOG_ERROR(">>>>>>>CustomFunction.SetValueByBit Arg ERROR!!!!!nNewBit Must be 0 or 1,{}", "");
        return nValue;
    }
    if (nBit > 31 || nBit < 0) {
        CONSTEXPR_LOG_ERROR(">>>>>>>CustomFunction.SetValueByBit Arg ERROR!!!!!BitIndex error{}", "");
        return nValue;
    }
    return (nValue & ~(1 << nBit)) | (nNewBitValue << nBit);
}

void lua::gfunc::RemoteCallToClient() {
    return;
}

int lua::gfunc::GetDistanceSq(int pX, int pY, int pZ, int tX, int tY, int tZ) {
    return (pX - tX) * (pX - tX) + (pY - tY) * (pY - tY) + (pZ - tZ) * (pZ - tZ);
}

int lua::gfunc::Random(int min, int max) {
    std::random_device              rd;
    std::mt19937                    gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return static_cast<int>(dis(gen));
}

std::string lua::gfunc::GetEditorString(int a, int b) {
    return std::to_string(a) + "-" + std::to_string(b);
}

bool lua::gfunc::IsClient() {
    return false;
}
