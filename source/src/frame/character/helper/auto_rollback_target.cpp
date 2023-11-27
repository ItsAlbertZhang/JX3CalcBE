#include "frame/character/helper/auto_rollback_target.h"

using namespace ns_frame;

AutoRollbackTarget::AutoRollbackTarget(Character *self, Character *target) {
    this->self = self;
    this->target = self->target;
    self->target = target;
}

AutoRollbackTarget::~AutoRollbackTarget() {
    self->target = this->target;
}