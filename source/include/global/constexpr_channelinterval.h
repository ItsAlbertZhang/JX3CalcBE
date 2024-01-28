#ifndef GLOBAL_CONSTEXPR_CHANNELINTERVAL_H_
#define GLOBAL_CONSTEXPR_CHANNELINTERVAL_H_

#ifdef D_CONSTEXPR_CHANNELINTERVAL

#include <mutex>
#include <unordered_map>

#ifdef _WIN32
#include <format>
namespace fmt = std;
#else
#include <fmt/core.h>
#endif

namespace nsgc_channelinterval {

class Item {

public:
    int    base            = 0;
    int    rand            = 0;
    double channelinterval = 0.0;
    bool   isBuff          = false;
};

// records 对顺序无要求, 因此使用线程锁
inline std::unordered_map<int, std::unordered_map<int, Item>> records;
inline std::mutex                                             mtx;
inline bool                                                   enable = false;

void record(int skillID, int skillLevel, int base, int rand, double channelinterval, bool isBuff);
void save();

} // namespace nsgc_channelinterval

#define CONSTEXPR_CHANNELINTERVAL_RECORD(skillID, skillLevel, base, rand, channelinterval, isBuff) \
    nsgc_channelinterval::record(skillID, skillLevel, base, rand, channelinterval, isBuff);

#else // D_CONSTEXPR_CHANNELINTERVAL

#define CONSTEXPR_CHANNELINTERVAL_RECORD(skillID, skillLevel, base, rand, channelinterval, isBuff)

#endif // D_CONSTEXPR_CHANNELINTERVAL

#endif // GLOBAL_CONSTEXPR_CHANNELINTERVAL_H_
