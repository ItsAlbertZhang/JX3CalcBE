#include "concrete.h"
#include "frame/character/effect.h"
#include <nlohmann/json.hpp>

using namespace jx3calc;
using namespace nlohmann;
using namespace frame::effect;

namespace {
extern const typeMap effect_map_hd;
} // namespace

template <>
auto concrete::createEffect<modules::config::ClientType::jx3_hd>(const std::string &type, const std::string &jsonstr) -> std::shared_ptr<frame::Effect> {
    return create(effect_map_hd, type, jsonstr);
}

namespace {
const typeMap effect_map_hd {
    {"套装·技能", [](frame::Character *obj) { switch(obj->kungfuID){
            case 10242: obj->skillrecipeAdd(948, 2); break;
            default: break;
    } }},
    {"套装·特效", [](frame::Character *obj) { switch(obj->kungfuID){
            case 10242: obj->skilleventAdd(1922); break;
            default: break;
    } }},
    {"大附魔·腰", tSkillevent {2623}},
    {"大附魔·腕", anyMap {{"风语-低品", tSkillevent {2663}}, {"风语-高品", tSkillevent {2673}}}},
    {"大附魔·鞋", anyMap {{"风语-低品", tSkillevent {2664}}, {"风语-高品", tSkillevent {2674}}}},
    {"腰坠·特效", anyMap {{"寻幽径", tItem {frame::ItemType::Trinket, 41362}}}},
    {"武器·特效",
     anyMap {
         {"大橙武", anyMultiple {tSkillrecipe {1542, 1}, tSkillrecipe {1543, 1}, tSkillevent {2421}, tSkillevent {1938}}},
         {"小橙武", anyMultiple {tSkillrecipe {1148, 1}, tSkillrecipe {1149, 1}}},
         {"阅世悲", tBuff {4761, 71, 1024, 10}},
     }},
    {"家园·酿造",
     anyMap {
         {"状元红", tBuff {17356, 1}},
         {"状元红·旬又三", tBuff {17358, 2}},
         {"女儿红", tBuff {17359, 1}},
         {"女儿红·旬又三", tBuff {17361, 2}},
     }},
    {"家园·烹饪",
     anyMap {
         {"煎豆腐", tBuff {27784, 1}},
         {"炖豆腐", tBuff {27785, 1}},
         {"小炒青菜", tBuff {27786, 1}},
         {"清蒸鲈鱼", tBuff {27787, 1}},
         {"炸鱼干", tBuff {27788, 1}},
     }},
    {"辅助类食品",
     anyMap {
         {"风语·鱼片砂锅粥", tBuff {29274, 3}},
         {"风语·鱼头豆腐汤", tBuff {29274, 8}},
     }},
    {"增强类食品",
     anyMap {
         {"风语·酸菜鱼", tBuff {29276, 1}},
         {"风语·红烧排骨", tBuff {29276, 2}},
         {"风语·白肉血肠", tBuff {29276, 3}},
         {"风语·红烧扣肉", tBuff {29276, 4}},
         {"风语·灌汤包", tBuff {29276, 6}},
         {"风语·鱼香肉丝", tBuff {29276, 9}},
         {"风语·水煮肉片", tBuff {29276, 10}},
         {"风语·毛血旺", tBuff {29276, 11}},
         {"风语·栗子烧肉", tBuff {29276, 12}},
         {"风语·鲜肉包子", tBuff {29276, 14}},
     }},
    {"辅助类药品",
     anyMap {
         {"风语·上品聚魂丹", tBuff {29288, 3}},
         {"风语·中品聚魂丹", tBuff {29288, 8}},
     }},
    {"增强类药品",
     anyMap {
         {"风语·上品玉璃散", tBuff {29289, 1}},
         {"风语·上品破秽散", tBuff {29289, 2}},
         {"风语·上品凝神散", tBuff {29289, 3}},
         {"风语·上品活气散", tBuff {29289, 4}},
         {"风语·上品展凤散", tBuff {29289, 6}},
         {"风语·中品玉璃散", tBuff {29289, 9}},
         {"风语·中品破秽散", tBuff {29289, 10}},
         {"风语·中品凝神散", tBuff {29289, 11}},
         {"风语·中品活气散", tBuff {29289, 12}},
         {"风语·中品展凤散", tBuff {29289, 14}},
     }},
    {"武器·熔锭",
     anyMap {
         {"风语·坠宵熔锭（内攻）", [](frame::Character *obj) { obj->chAttr.atMagicAttackPowerBase += 949; }},
         {"风语·坠宵磨石（内攻）", [](frame::Character *obj) { obj->chAttr.atMagicAttackPowerBase += 475; }},
     }},
    {"宴席", anyMap {{"风语·二十四桥明月夜", tBuff {29284, 2}}}},
    {"同泽宴", tBuff {18428, 1}},
    {"蒸鱼菜盘", tBuff {2563, 1, 1024}},
    {"水煮鱼", anyMap {{"炼狱水煮鱼", tBuff {10100, 1}}, {"百炼水煮鱼", tBuff {10100, 2}}}},
    {"吟冬卧雪", tBuff {18811, 1}}, // {stacknum: int}
    {"戒火", tBuff {4058, 1, 1024, 1, 0, true}},
    {"立地成佛", tBuff {566, 1, 1024, 5, 0, true}},
    {"袖气", tBuff {673, 11}},
    {"弘法", tBuff {10208, 1}}, // {stacknum: int, covrate: double}
    {"号令三军",
     tBuff {23107, 1, 1, 1, 0, false, // {stacknum: int}
            [](frame::Character *obj) {
                auto buff = obj->buffGet(23107, 1);
                if (buff && buff->isValid) {
                    buff->nCustomValue = 2;
                }
            }}},
    {"朝圣言", // {name:string, stacknum: int, covrate: double}
     anyMap {
         {"朝圣", tBuff {4246, 1, 8}},
         {"圣浴明心", tBuff {9744, 1, 8}},
     }},
    {"振奋", tBuff {8504, 1}},      // {stacknum: int, covrate: double}
    {"寒啸千军", tBuff {10031, 1}}, // {covrate: double}
    {"秋肃", tBuff {29294, 1}},     // {stacknum: int, covrate: double}
    {"皎素", tBuff {24350, 1}},     // {covrate: double}
    {"左旋右转", tBuff {20938, 1}}, // {stacknum: int, covrate: double}
    {"仙王蛊鼎", tBuff {24742, 1}}, // {stacknum: int, covrate: double}
    {"庄周梦", tBuff {23543, 1}},   // {stacknum: int, covrate: double}
    // {"飘黄", tBuff {20854, 1}},     // {covrate: double}
    {"配伍", tBuff {20877, 1}}, // {stacknum: int, covrate: double}
    {"破苍穹", tBuff {375, 5}}, // {covrate: double}
    {"疏狂", tBuff {11456, 1}}, // {covrate: double}
    {"小队阵法",
     anyMap {
         {"易筋经·天鼓雷音阵", anyMultiple {tBuff {919, 6, 1024}, tBuff {920, 1, 1, 5, 1.0}}},
         {"花间游·七绝逍遥阵", tBuff {934, 6, 1024}},
         {"紫霞功·九宫八卦阵", anyMultiple {tBuff {938, 6, 1024}, tBuff {943, 1, 1, 5, 1.0}}},
         {"毒经·万蛊噬心阵", anyMultiple {tBuff {2512, 6, 1024}, tBuff {2514, 1, 1, 1, 1.0}}},
         {"天罗诡道·千机百变阵", anyMultiple {tBuff {3307, 6, 1024}, tBuff {3310, 1, 1, 1, 1.0}}},
         {"莫问·万籁金弦阵", anyMultiple {tBuff {9485, 1, 1024}, tBuff {9492, 1, 1, 1, 1.0}}},
         {"山海心诀·苍梧引灵阵", anyMultiple {tBuff {27236, 6, 1024}, tBuff {27238, 1, 1, 1, 1.0}}},
         {"周天功·含章挺秀阵", anyMultiple {tBuff {29481, 6, 1024}, tBuff {29482, 1, 1, 1, 1.0}}},
         {"无界端·无界行侠阵", [](frame::Character *obj) { obj->chAttr.atAllDamageAddPercent += 51; obj->chAttr.atStrainRate += 102; }}
     }}
};
} // namespace

void concrete::dummyFunc1() {
    return;
} // fix unused includes warning
