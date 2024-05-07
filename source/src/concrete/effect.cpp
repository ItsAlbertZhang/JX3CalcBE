#include "frame/character/effect.h"
#include "frame/common/item.h"
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

using namespace jx3calc;
using namespace nlohmann;

namespace {

enum class Type {
    套装·技能,
    套装·特效,
    大附魔·腰,
    大附魔·腕,
    大附魔·鞋,
    腰坠·特效,
    武器·特效,
    家园·酿造,
};

inline const std::unordered_map<std::string, Type> typeMap = {
    {"套装·技能", Type::套装·技能}, // bool
    {"套装·特效", Type::套装·特效}, // bool
    {"大附魔·腰", Type::大附魔·腰}, // string
    {"大附魔·腕", Type::大附魔·腕}, // string
    {"大附魔·鞋", Type::大附魔·鞋}, // string
    {"腰坠·特效", Type::腰坠·特效}, // string
    {"武器·特效", Type::武器·特效}, // string
    {"家园·酿造", Type::家园·酿造}, // string
};

/*
套装·技能: bool
套装·特效: bool
大附魔·腰: bool
大附魔·腕: string: ["万灵当歌" | "雾海寻龙"]
大附魔·鞋: string: ["万灵当歌" | "雾海寻龙"]
腰坠·特效: string: ["吹香雪" | "梧桐影"]
武器·特效: string: ["血月" | "封霜曲刃·忆" | "冰焰玉" | "无尽沙海"]
家园·酿造: string: ["女儿红"]
*/

template <Type type, typename T>
class Effect : public frame::Effect {
public:
    Effect(T value)
        : value(std::move(value)) {}
    virtual void active(frame::Character *obj) const override;

private:
    T value;
};

template <Type type>
auto get(const json &value) -> std::shared_ptr<frame::Effect>;

// 套装·技能
template <>
void Effect<Type::套装·技能, int>::active(frame::Character *obj) const {
    switch (obj->kungfuID) {
    case 10242: // 焚影圣诀
        obj->skillrecipeAdd(948, 2);
        break;
    default:
        break;
    }
}
template <>
auto get<Type::套装·技能>(const json &value) -> std::shared_ptr<frame::Effect> {
    if (!value.is_boolean() || !value.get<bool>())
        return nullptr;
    return std::make_shared<Effect<Type::套装·技能, int>>(0);
}

// 套装·特效
template <>
void Effect<Type::套装·特效, int>::active(frame::Character *obj) const {
    switch (obj->kungfuID) {
    case 10242: // 焚影圣诀
        obj->skilleventAdd(1922);
        break;
    default:
        break;
    }
}
template <>
auto get<Type::套装·特效>(const json &value) -> std::shared_ptr<frame::Effect> {
    if (!value.is_boolean() || !value.get<bool>())
        return nullptr;
    return std::make_shared<Effect<Type::套装·特效, int>>(0);
}

// 大附魔·腰
template <>
void Effect<Type::大附魔·腰, int>::active(frame::Character *obj) const {
    obj->skilleventAdd(2623);
}
template <>
auto get<Type::大附魔·腰>(const json &value) -> std::shared_ptr<frame::Effect> {
    if (!value.is_boolean() || !value.get<bool>())
        return nullptr;
    return std::make_shared<Effect<Type::大附魔·腰, int>>(0);
}

// 大附魔·腕
template <>
void Effect<Type::大附魔·腕, int>::active(frame::Character *obj) const {
    obj->skilleventAdd(value);
}
template <>
auto get<Type::大附魔·腕>(const json &value) -> std::shared_ptr<frame::Effect> {
    static const std::unordered_map<std::string, int> map{
        {"万灵当歌", 2554},
        {"雾海寻龙", 2624},
    };
    if (!value.is_string() || !map.contains(value.get<std::string>()))
        return nullptr;
    return std::make_shared<Effect<Type::大附魔·腕, int>>(map.at(value.get<std::string>()));
}

// 大附魔·鞋
template <>
void Effect<Type::大附魔·鞋, int>::active(frame::Character *obj) const {
    obj->skilleventAdd(value);
}
template <>
auto get<Type::大附魔·鞋>(const json &value) -> std::shared_ptr<frame::Effect> {
    static const std::unordered_map<std::string, int> map{
        {"万灵当歌", 2555},
        {"雾海寻龙", 2625},
    };
    if (!value.is_string() || !map.contains(value.get<std::string>()))
        return nullptr;
    return std::make_shared<Effect<Type::大附魔·鞋, int>>(map.at(value.get<std::string>()));
}

// 腰坠·特效
template <>
void Effect<Type::腰坠·特效, int>::active(frame::Character *obj) const {
    obj->itemAdd(frame::ItemType::Trinket, value);
}
template <>
auto get<Type::腰坠·特效>(const json &value) -> std::shared_ptr<frame::Effect> {
    static const std::unordered_map<std::string, int> map{
        {"吹香雪", 38789},
        {"梧桐影", 39853},
    };
    if (!value.is_string() || !map.contains(value.get<std::string>()))
        return nullptr;
    return std::make_shared<Effect<Type::腰坠·特效, int>>(map.at(value.get<std::string>()));
}

// 武器·特效
enum class 武器特效 {
    血月,
    封霜曲刃·忆,
    冰焰玉,
    无尽沙海,
};
template <>
void Effect<Type::武器·特效, 武器特效>::active(frame::Character *obj) const {
    switch (value) {
    case 武器特效::血月:
        obj->skillrecipeAdd(1542, 1); // 驱夜伤害提高 5%
        obj->skillrecipeAdd(1543, 1); // 净世破魔击伤害提高 5%
        obj->skilleventAdd(2421);     // 触发特效
        obj->skilleventAdd(1938);     // 施展驱夜断愁造成少量伤害
        break;
    case 武器特效::封霜曲刃·忆:
        obj->skillrecipeAdd(1148, 1); // 月斩会心提高 5%
        obj->skillrecipeAdd(1149, 1); // 日斩会心提高 5%
        break;
    case 武器特效::冰焰玉:
        obj->chAttr.atMagicAttackPowerBase += 1170; // 4761, 63
        break;
    case 武器特效::无尽沙海:
        obj->chAttr.atMagicAttackPowerBase += 1340; // 4761, 67
        break;
    }
}
template <>
auto get<Type::武器·特效>(const json &value) -> std::shared_ptr<frame::Effect> {
    static const std::unordered_map<std::string, 武器特效> map{
        {"血月", 武器特效::血月},
        {"封霜曲刃·忆", 武器特效::封霜曲刃·忆},
        {"冰焰玉", 武器特效::冰焰玉},
        {"无尽沙海", 武器特效::无尽沙海},
    };
    if (!value.is_string() || !map.contains(value.get<std::string>()))
        return nullptr;
    return std::make_shared<Effect<Type::武器·特效, 武器特效>>(map.at(value.get<std::string>()));
}

// 使用类物品的额外属性
enum class ItemAddType {
    haste,
};

// 家园·酿造
template <>
void Effect<Type::家园·酿造, ItemAddType>::active(frame::Character *obj) const {
    switch (value) {
    case ItemAddType::haste:
        obj->chAttr.atHasteBase += 1144; // 17361, 2
        break;
    }
}
template <>
auto get<Type::家园·酿造>(const json &value) -> std::shared_ptr<frame::Effect> {
    static const std::unordered_map<std::string, ItemAddType> map{
        {"女儿红", ItemAddType::haste},
    };
    if (!value.is_string() || !map.contains(value.get<std::string>()))
        return nullptr;
    return std::make_shared<Effect<Type::家园·酿造, ItemAddType>>(map.at(value.get<std::string>()));
}

} // namespace

auto concrete::createEffect(const std::string &type, const std::string &jsonstr) -> std::shared_ptr<frame::Effect> {
    if (!typeMap.contains(type))
        return nullptr;
    const auto value = json::parse(jsonstr);
    switch (typeMap.at(type)) {
    case Type::大附魔·腰:
        return get<Type::大附魔·腰>(value);
    case Type::大附魔·腕:
        return get<Type::大附魔·腕>(value);
    case Type::大附魔·鞋:
        return get<Type::大附魔·鞋>(value);
    case Type::套装·技能:
        return get<Type::套装·技能>(value);
    case Type::套装·特效:
        return get<Type::套装·特效>(value);
    case Type::腰坠·特效:
        return get<Type::腰坠·特效>(value);
    case Type::武器·特效:
        return get<Type::武器·特效>(value);
    case Type::家园·酿造:
        return get<Type::家园·酿造>(value);
    default:
        return nullptr;
    }
}
