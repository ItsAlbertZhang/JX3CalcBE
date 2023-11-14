#ifdef D_CONSTEXPR_CHANNELINTERVAL

#include "plugin/channelinterval.h"
#include "frame/global/buff.h"
#include "frame/global/skill.h"
#include "modules/config.h"
#include <format>
#include <fstream>

using namespace jx3calc;

void plugin::channelinterval::record(int id, int level, int base, int rand, double channelinterval, bool isBuff) {
    if (enable) {
        records[id][level] = {base, rand, channelinterval, isBuff};
    }
}

void plugin::channelinterval::save() {
    std::ofstream file(modules::config::pExeDir / "output_channelinterval.tab");
    file << std::format("Base\tRand\tCoefficient\tSkillID\tSkillLevel\tName\n");
    for (const auto &[id, skillLevelMap] : records) {
        for (const auto &[level, item] : skillLevelMap) {
            std::string name;
            if (item.isBuff) {
                name = frame::BuffManager::get(id, level).Name;
            } else {
                name = frame::SkillManager::get(id, level).Name;
            }
            file << std::format("{}\t{}\t{:.4f}\t{}\t{}\t{}\n", item.base, item.rand, item.channelinterval, id, level, name);
        }
    }
    file.close();
    enable = false;
}

#endif // D_CONSTEXPR_CHANNELINTERVAL
