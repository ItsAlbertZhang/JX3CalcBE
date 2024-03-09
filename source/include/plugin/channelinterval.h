#ifndef GLOBAL_CONSTEXPR_CHANNELINTERVAL_H_
#define GLOBAL_CONSTEXPR_CHANNELINTERVAL_H_

#ifdef D_CONSTEXPR_CHANNELINTERVAL

#include <unordered_map>

namespace ns_plugin {
namespace channelinterval {

class Item {

public:
    int    base            = 0;
    int    rand            = 0;
    double channelinterval = 0.0;
    bool   isBuff          = false;
};

inline std::unordered_map<int, std::unordered_map<int, Item>> records;
inline bool                                                   enable = true;

void record(int skillID, int skillLevel, int base, int rand, double fChannelInterval, bool isBuff);
void save();

} // namespace channelinterval
} // namespace ns_plugin

#define CONSTEXPR_CHANNELINTERVAL_RECORD(skillID, skillLevel, base, rand, fChannelInterval, isBuff) \
    ns_plugin::channelinterval::record(skillID, skillLevel, base, rand, fChannelInterval, isBuff);

#else // D_CONSTEXPR_CHANNELINTERVAL

#define CONSTEXPR_CHANNELINTERVAL_RECORD(skillID, skillLevel, base, rand, fChannelInterval, isBuff)

#endif // D_CONSTEXPR_CHANNELINTERVAL

#endif // GLOBAL_CONSTEXPR_CHANNELINTERVAL_H_
