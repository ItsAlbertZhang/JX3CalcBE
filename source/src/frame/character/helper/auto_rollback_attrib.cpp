#include "frame/character/helper/auto_rollback_attrib.h"
#include "frame/static_refmap.h" // enumTabAttribute
#include "program/log.h"

using namespace ns_frame;
using namespace ns_framestatic;

AutoRollbackAttrib::AutoRollbackAttrib(Character *self, const Buff &buff) : self(self), buff(buff) {
    for (const auto &it : buff.BeginAttrib) {
        handle(it, false);
    }
}

AutoRollbackAttrib::~AutoRollbackAttrib() {
    for (const auto &it : buff.BeginAttrib) {
        handle(it, true);
    }
    for (const auto &it : buff.EndTimeAttrib) {
        handle(it, false);
    }
}
void AutoRollbackAttrib::active() {
    for (const auto &it : buff.ActiveAttrib) {
        handle(it, false);
    }
}

void AutoRollbackAttrib::handle(const Buff::Attrib &attrib, bool isRollback) {
    int c = isRollback ? -1 : 1;
    switch (attrib.type) {
    case enumTabAttribute::atLunarDamageCoefficient:
        self->chAttr.atLunarDamageCoefficient += c * attrib.valueAInt;
        break;
    case enumTabAttribute::atSolarDamageCoefficient:
        self->chAttr.atSolarDamageCoefficient += c * attrib.valueAInt;
        break;
    default:
        LOG_ERROR("Undefined: Unknown Attribute: %s\n", refTabAttribute[static_cast<int>(attrib.type)].c_str());
        break;
    }
}