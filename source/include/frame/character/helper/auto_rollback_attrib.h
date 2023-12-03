#ifndef FRAME_CHARACTER_HELPER_AUTO_ROLLBACK_ATTRIB_H_
#define FRAME_CHARACTER_HELPER_AUTO_ROLLBACK_ATTRIB_H_

#include "frame/character/character.h"
#include "frame/global/buff.h"

namespace ns_frame {

/**
 * @brief 自动回滚的 buff 属性
 * @param self 自身
 * @param buff buff
 * @note 当 AutoRollbackAttrib 对象销毁时, 会自动将 BeginAttrib 回滚到原来的值.
 */
class AutoRollbackAttrib {
public:
    AutoRollbackAttrib(Character *self, CharacterBuff::Item *item, const Buff &buff);
    ~AutoRollbackAttrib();
    void active();

private:
    Character *self;
    CharacterBuff::Item *item;
    const Buff &buff;

    void handle(CharacterBuff::Item *item, const Buff::Attrib &attrib, bool isRollback);
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_HELPER_AUTO_ROLLBACK_ATTRIB_H_