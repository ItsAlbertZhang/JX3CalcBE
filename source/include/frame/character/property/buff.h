#ifndef FRAME_CHARACTER_PROPERTY_BUFF_H_
#define FRAME_CHARACTER_PROPERTY_BUFF_H_

#include "frame/global/buff.h"

namespace ns_frame {

class Character; // 前置声明

class CharacterBuff : public Buff {
public:
    int tickOver = 0; // 结束 tick
    int nStackNum = 0;
    Character *source = nullptr;
};

} // namespace ns_frame

#endif // FRAME_CHARACTER_PROPERTY_BUFF_H_