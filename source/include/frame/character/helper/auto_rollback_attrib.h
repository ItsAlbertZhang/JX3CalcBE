#ifndef FRAME_CHARACTER_HELPER_AUTO_ROLLBACK_ATTRIB_H_
#define FRAME_CHARACTER_HELPER_AUTO_ROLLBACK_ATTRIB_H_

#include "frame/character/property/buff.h"
#include "frame/global/buff.h"

namespace jx3calc {
namespace frame {

class Character;

/**
 * @brief 自动回滚的 buff 属性
 * @param self 自身
 * @param buff buff
 * @note 当 AutoRollbackAttrib 对象销毁时, 会自动将 BeginAttrib 回滚到原来的值.
 */
class AutoRollbackAttrib {
public:
    AutoRollbackAttrib(Character *self, BuffItem *item, const Buff &buff);
    AutoRollbackAttrib(const AutoRollbackAttrib &)            = delete;
    AutoRollbackAttrib &operator=(const AutoRollbackAttrib &) = delete;
    AutoRollbackAttrib(AutoRollbackAttrib &&)                 = delete;
    AutoRollbackAttrib &operator=(AutoRollbackAttrib &&)      = delete;
    ~AutoRollbackAttrib();

    void active();

private:
    Character  *self;
    BuffItem   *item;
    const Buff &buff;

    void handle(const Buff::Attrib &attrib, bool isRollback);
};

} // namespace frame
} // namespace jx3calc

#endif // FRAME_CHARACTER_HELPER_AUTO_ROLLBACK_ATTRIB_H_
