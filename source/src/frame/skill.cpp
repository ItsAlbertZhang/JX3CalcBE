#include "frame/skill.h"

using namespace ns_frame;

// 静态成员变量初始化
std::unordered_map<int, std::unordered_map<int, Skill>> CacheSkill::data;

// void CacheSkill::init(int skillID, int skillLevel) {
//     // 若技能 ID 不存在, 则创建新的技能 ID, 并从 GameDataFetcher 中获取技能数据
//     if (data.find(skillID) == data.end()) {
//         data[skillID] = std::unordered_map<int, Skill>();
//         data[skillID][skillLevel] = fetchData(skillID);
//     } else {
//         // 若技能 ID 存在, 但技能等级不存在, 则复用同 ID 技能数据的 GetSkillLevelData 函数
//         if (data[skillID].find(skillLevel) == data[skillID].end()) {
//             // coding...
//         }
//     }
// }

// Skill &CacheSkill::fetchData(int skillID) {
//     // coding...
// }