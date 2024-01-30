#ifdef D_CONSTEXPR_CHANNELINTERVAL

#include "plugin/channelinterval.h"
#include "frame/global/uibuff.h"
#include "frame/global/uiskill.h"
#include "utils/config.h"
#include <fstream>

void ns_plugin::channelinterval::record(int skillID, int skillLevel, int base, int rand, double channelinterval, bool isBuff) {
    if (enable) {
        std::lock_guard<std::mutex> lock(mtx);
        records[skillID][skillLevel] = {base, rand, channelinterval, isBuff};
    }
}

void ns_plugin::channelinterval::save() {
    std::ofstream file(ns_utils::config::pExeDir / "output_channelinterval.tab");
    file << fmt::format("Base\tRand\tCoefficient\tSkillID\tSkillLevel\tName\n");
    for (const auto &[skillID, skillLevelMap] : records) {
        for (const auto &[skillLevel, item] : skillLevelMap) {
            std::string name;
            if (item.isBuff) {
                name = ns_frame::UIBuffManager::get(skillID, skillLevel).Name;
            } else {
                name = ns_frame::UISkillManager::get(skillID, skillLevel).Name;
            }
            file << fmt::format("{}\t{}\t{:.4f}\t{}\t{}\t{}\n", item.base, item.rand, item.channelinterval, skillID, skillLevel, name);
        }
    }
    file.close();
}

#endif // D_CONSTEXPR_CHANNELINTERVAL
