#include "frame/character/character.h"
#include "frame/character/helper/auto_rollback_attrib.h"
#include "frame/global/buff.h"

using namespace ns_frame;

void Character::AddBuff(int buffSourceID, int buffSourceLevel, int buffID, int buffLevel) {
    printf("%d AddBuff: %d %d %d %d\n", this->dwID, buffSourceID, buffSourceLevel, buffID, buffLevel);
    if (this->chBuff.buffList[buffSourceID][buffID].find(buffLevel) == this->chBuff.buffList[buffSourceID][buffID].end()) {
        // 对于 buffSourceID 和 buffID, 直接使用 [] 运算符, 没有则直接创建.
        this->chBuff.buffList[buffSourceID][buffID].emplace(
            std::piecewise_construct,
            std::forward_as_tuple(buffLevel),
            std::forward_as_tuple(this->dwID, buffSourceID, buffID, buffLevel));
        // 原地插入. 通过这种方式插入的 key 和 value, 不是构造后移动至容器, 而是直接在容器内构造. 这可以避免 Item 的 const 属性出现问题.
    }
    CharacterBuff::Item &it = this->chBuff.buffList[buffSourceID][buffID].at(buffLevel);
    const Buff &buff = BuffManager::get(buffID, buffLevel);
    if (it.isValid) {
        // TODO: buff 当前有效
    } else {
        it.isValid = true;
        it.ptrAttrib = new AutoRollbackAttrib(this, buff);
        // TODO: 后续逻辑
    }
}