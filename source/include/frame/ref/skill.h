#pragma once

#include "frame/ref/ref.h"

namespace jx3calc {
namespace frame {

enum class ref::Skill::KindType {
    None,
    Physics,
    SolarMagic,
    LunarMagic,
    NeutralMagic,
    Poison,
    Leap,
    Adaptive,
};

enum class ref::Skill::CastMode {
    CasterArea,
    CasterAreaOfAttention,
    CasterAreaOfDepth,
    CasterConvexHullArea,
    CasterSingle,
    CasterSpreadCircle,
    Item,
    PartyArea,
    Point,
    PointArea,
    PointAreaFindFirst,
    PointAreaOfCasterTeam,
    PointRectangle,
    Rectangle,
    RectangleOfDepth,
    Sector,
    SectorOfAttention,
    SectorOfDepth,
    TargetAngleRectangle,
    TargetAngleSector,
    TargetArea,
    TargetChain,
    TargetHoodle,
    TargetLeader,
    TargetRay,
    TargetSingle,
    TargetTeamArea,
};

} // namespace frame
} // namespace jx3calc
