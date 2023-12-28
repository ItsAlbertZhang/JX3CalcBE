#ifndef THREAD_SHARE_P_ARG_H_
#define THREAD_SHARE_P_ARG_H_

#include "frame/character/derived/player.h"
#include "frame/character/property/attribute.h"
#include <string>

namespace ns_thread {

class PArg {
public:
    const ns_frame::ChAttr attr_backup;

    const int delay_network = 45;
    const int delay_keybord = 20;
    const int fight_count   = 100;
    const int fight_time    = 300;

    const bool tz_jn  = false;
    const bool tz_tx  = false;
    const bool dfm_yd = false;
    const bool dfm_hw = false;
    const bool dfm_xz = false;
    const bool wq_cw  = false;

    const bool        use_custom_macro = false;
    const std::string custom_macro;
};

} // namespace ns_thread

#endif // THREAD_SHARE_P_ARG_H_
