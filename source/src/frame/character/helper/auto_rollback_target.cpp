#include "frame/character/helper/auto_rollback_target.h"

using namespace ns_frame;

AutoRollbackTarget::AutoRollbackTarget(Character *self) {
    this->self = self;
    this->preTarget = self->targetCurr;
}

AutoRollbackTarget::~AutoRollbackTarget() {
    self->targetCurr = this->preTarget;
}