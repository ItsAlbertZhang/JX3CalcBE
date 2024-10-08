#pragma once

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
class HelperBuff {
public:
    HelperBuff(Character *self, BuffItem *item, const Buff &buff);
    HelperBuff(const HelperBuff &)            = delete;
    HelperBuff &operator=(const HelperBuff &) = delete;
    HelperBuff(HelperBuff &&)                 = delete;
    HelperBuff &operator=(HelperBuff &&)      = delete;
    ~HelperBuff();

    void active();
    void load();
    void unload();

private:
    Character  *self;
    BuffItem   *item;
    const Buff &buff;

    void handle(const Buff::Attrib &attrib, bool isRollback);
};

} // namespace frame
} // namespace jx3calc
