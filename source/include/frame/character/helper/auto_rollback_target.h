#ifndef FRAME_CHARACTER_HELPER_AUTO_ROLLBACK_TARGET_H_
#define FRAME_CHARACTER_HELPER_AUTO_ROLLBACK_TARGET_H_

#include "frame/character/character.h"

namespace ns_frame {

/**
 * @brief 自动回滚的目标切换
 * @param self 自身
 * @param target 新目标
 * @note 当 AutoRollbackTarget 对象销毁时, 会自动将 self->target 回滚到原来的值.
 */
class AutoRollbackTarget {
public:
    AutoRollbackTarget(Character *self, Character *target);
    ~AutoRollbackTarget();

private:
    Character *self;
    Character *target;
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_HELPER_AUTO_ROLLBACK_TARGET_H_