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
    {"大附魔·腕", anyMap {{"万灵当歌", tSkillevent {2554}}, {"雾海寻龙", tSkillevent {2624}}}},
    {"大附魔·鞋", anyMap {{"万灵当歌", tSkillevent {2555}}, {"雾海寻龙", tSkillevent {2625}}}},
    {"腰坠·特效", anyMap {{"吹香雪", tItem {frame::ItemType::Trinket, 38789}}, {"梧桐影", tItem {frame::ItemType::Trinket, 39853}}}},
    {"武器·特效",
     anyMap {
         {"血月", anyMultiple {tSkillrecipe {1542, 1}, tSkillrecipe {1543, 1}, tSkillevent {2421}, tSkillevent {1938}}},
         {"封霜曲刃·忆", anyMultiple {tSkillrecipe {1148, 1}, tSkillrecipe {1149, 1}}},
         {"无尽沙海", tBuff {4761, 67, 1024, 10}},
         {"冰焰玉", tBuff {4761, 63, 1024, 10}},
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
         {"鱼片砂锅粥", tBuff {24731, 3}},
         {"鱼头豆腐汤", tBuff {24731, 8}},
     }},
    {"增强类食品",
     anyMap {
         {"酸菜鱼", tBuff {24732, 1}},
         {"红烧排骨", tBuff {24732, 2}},
         {"白肉血肠", tBuff {24732, 3}},
         {"红烧扣肉", tBuff {24732, 4}},
         {"灌汤包", tBuff {24732, 6}},
         {"鱼香肉丝", tBuff {24732, 9}},
         {"水煮肉片", tBuff {24732, 10}},
         {"毛血旺", tBuff {24732, 11}},
         {"栗子烧肉", tBuff {24732, 12}},
         {"鲜肉包子", tBuff {24732, 14}},
     }},
    {"辅助类药品",
     anyMap {
         {"上品聚魂丹", tBuff {24735, 3}},
         {"中品聚魂丹", tBuff {24735, 8}},
     }},
    {"增强类药品",
     anyMap {
         {"上品玉璃散", tBuff {24736, 1}},
         {"上品破秽散", tBuff {24736, 2}},
         {"上品凝神散", tBuff {24736, 3}},
         {"上品活气散", tBuff {24736, 4}},
         {"上品展凤散", tBuff {24736, 6}},
         {"中品玉璃散", tBuff {24736, 9}},
         {"中品破秽散", tBuff {24736, 10}},
         {"中品凝神散", tBuff {24736, 11}},
         {"中品活气散", tBuff {24736, 12}},
         {"中品展凤散", tBuff {24736, 14}},
     }},
    {"武器·熔锭",
     anyMap {
         {"断浪·坠宵熔锭（内攻）", [](frame::Character *obj) { obj->chAttr.atMagicAttackPowerBase += 786; }},
         {"断浪·坠宵磨石（内攻）", [](frame::Character *obj) { obj->chAttr.atMagicAttackPowerBase += 393; }},
     }},
    {"宴席", anyMap {{"二十四桥明月夜", tBuff {24733, 2}}}},
    {"同泽宴", tBuff {18428, 1}},
    {"蒸鱼菜盘", tBuff {2563, 1, 1024}},
    {"水煮鱼", anyMap {{"炼狱水煮鱼", tBuff {10100, 1}}, {"百炼水煮鱼", tBuff {10100, 2}}}},
    {"吟冬卧雪", tBuff {18811, 1}}, // {stacknum: int}
    {"目标易伤",
     anyMap {
         {"戒火", tBuff {4058, 1, 1024, 1, 0, true}},
         {"秋肃", tBuff {23305, 1, 1024, 1, 0, true}},
     }},
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
    {"寒啸千军", tBuff {10031, 1}}, // {stacknum: int, covrate: double}
    {"皎素", tBuff {24350, 1}},     // {covrate: double}
    {"左旋右转", tBuff {20938, 1}}, // {stacknum: int, covrate: double}
    {"仙王蛊鼎", tBuff {24742, 1}}, // {covrate: double}
    {"庄周梦", tBuff {23543, 1}},   // {stacknum: int, covrate: double}
    {"飘黄", tBuff {20854, 1}},     // {covrate: double}
    {"配伍", tBuff {20877, 1}},     // {stacknum: int, covrate: double}
    {"破苍穹", tBuff {375, 5}},     // {covrate: double}
    {"疏狂", tBuff {11456, 1}},     // {covrate: double}
    {"小队阵法",
     anyMap {
         {"易筋经·天鼓雷音阵", anyMultiple {tBuff {919, 6, 1024}, tBuff {920, 1, 1, 5, 1.0}}},
         {"花间游·七绝逍遥阵", tBuff {934, 6, 1024}},
         {"紫霞功·九宫八卦阵", anyMultiple {tBuff {938, 6, 1024}, tBuff {943, 1, 1, 5, 1.0}}},
         {"毒经·万蛊噬心阵", anyMultiple {tBuff {2512, 6, 1024}, tBuff {2514, 1, 1, 1, 1.0}}},
         {"天罗诡道·千机百变阵", anyMultiple {tBuff {3307, 6, 1024}, tBuff {3310, 1, 1, 1, 1.0}}},
         {"莫问·万籁金弦阵", anyMultiple {tBuff {9485, 1, 1024}, tBuff {9492, 1, 1, 1, 1.0}}},
         {"山海心诀·苍梧引灵阵", anyMultiple {tBuff {27236, 6, 1024}, tBuff {27238, 1, 1, 1, 1.0}}},
         {"无界端·无界行侠阵", [](frame::Character *obj) { obj->chAttr.atAllDamageAddPercent += 51; obj->chAttr.atStrainRate += 102; }}
     }}
};
} // namespace

void concrete::dummyFunc() {
    return;
} // fix unused includes warning
