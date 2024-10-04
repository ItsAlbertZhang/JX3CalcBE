#include "frame/character/effect.h"
#include "frame/event.h"
#include "nlohmann/json.hpp"

using namespace jx3calc;
using namespace frame;

void Effect::active(Character *obj) {
    using namespace effect;
    const auto visitorAny = [this, &obj](auto &&arg) {
        const auto visitorAnySingle = [this, &obj](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            // tBuff, tItem, tSkillevent, tSkillrecipe
            if constexpr (std::is_same_v<T, tBuff>) {
                Character *it = (arg.target && obj->targetSelect) ? obj->targetSelect : obj;
                it->buffAdd(0, 99, arg.id, arg.level, arg.count, 0, arg.stacknum);
                if (arg.covrate > 0) {
                    const auto delay = static_cast<event_tick_t>(it->buffTimeLeftTick(arg.id, arg.level) / arg.covrate);
                    const auto cb    = [](void *e, void *o) {
                        auto effect = static_cast<Effect *>(e);
                        auto obj    = static_cast<Character *>(o);
                        effect->active(obj);
                    };
                    Event::add(delay, cb, this, obj);
                }
                if (arg.add) {
                    arg.add(obj);
                }
            } else if constexpr (std::is_same_v<T, tItem>) {
                obj->itemAdd(arg.type, arg.id);
            } else if constexpr (std::is_same_v<T, tSkillevent>) {
                obj->skilleventAdd(arg.id);
            } else if constexpr (std::is_same_v<T, tSkillrecipe>) {
                obj->skillrecipeAdd(arg.id, arg.level);
            }
        };
        using T = std::decay_t<decltype(arg)>;
        // anySingle, anyMultiple
        if constexpr (std::is_same_v<T, anySingle>) {
            std::visit(visitorAnySingle, arg);
        } else if constexpr (std::is_same_v<T, anyMultiple>) {
            for (const auto &item : arg) {
                std::visit(visitorAnySingle, item);
            }
        } else if constexpr (std::is_same_v<T, anyFunc>) {
            arg(obj);
        }
    };
    std::visit(visitorAny, value);
}

auto effect::create(const effect::typeMap &effect_map, const std::string &type, const std::string &jsonstr) -> std::shared_ptr<Effect> {
    if (!effect_map.contains(type)) {
        return nullptr;
    }
    const auto &effect  = effect_map.at(type);
    const auto  value   = nlohmann::json::parse(jsonstr);
    const auto  visitor = [&value](auto &&arg) -> std::shared_ptr<Effect> {
        const auto visitorAny = [&value](auto &&arg) -> std::shared_ptr<Effect> {
            const auto visitorAnySingle = [&value](auto &&arg) -> anySingle {
                auto it = arg; // copy
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, tBuff>) {
                    if (value.is_object()) {
                        if (value.contains("stacknum") && value.at("stacknum").is_number_integer()) {
                            it.stacknum = value.at("stacknum").get<int>();
                            it.stacknum = std::max(1, it.stacknum);
                        }
                        if (value.contains("covrate") && value.at("covrate").is_number()) {
                            it.covrate = value.at("covrate").get<double>();
                            it.covrate = std::max(0.0, it.covrate);
                            it.covrate = std::min(1.0, it.covrate);
                        }
                    }
                }
                return it;
            };
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, anySingle>) {
                return std::make_shared<Effect>(std::visit(visitorAnySingle, arg));
            } else if constexpr (std::is_same_v<T, anyMultiple>) {
                anyMultiple it;
                for (const auto &item : arg) {
                    it.emplace_back(std::visit(visitorAnySingle, item));
                }
                return std::make_shared<Effect>(it);
            } else { // anyFunc 不允许自定义参数
                return std::make_shared<Effect>(arg);
            }
        };
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, any>) {
            return std::visit(visitorAny, arg);
        } else if constexpr (std::is_same_v<T, anyMap>) {
            std::string name;
            if (value.is_string()) {
                name = value.get<std::string>();
            } else if (value.is_object() && value.contains("name") && value.at("name").is_string()) {
                name = value.at("name").get<std::string>();
            }
            if (arg.contains(name))
                return std::visit(visitorAny, arg.at(name));
            else
                return nullptr;
        } else
            return nullptr;
    };
    return std::visit(visitor, effect);
}
