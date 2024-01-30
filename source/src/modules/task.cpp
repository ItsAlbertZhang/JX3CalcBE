#include "modules/task.h"
#include "concrete/character/character.h"
#include "concrete/effect/effect.h"
#include "frame/character/derived/player.h"
#include "plugin/log.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <vector>

#define UNREFERENCED_PARAMETER(P) (P)

using namespace ns_modules;

std::string DMTask::format() {
    using json = nlohmann::json;
    json j{
        {"player",        json::array()                           },
        {"delayNetwork",  {nDelayNetwork.min, nDelayNetwork.max}  },
        {"delayKeyboard", {nDelayKeyboard.min, nDelayKeyboard.max}},
        {"fightTime",     {nFightTime.min, nFightTime.max}        },
        {"fightCount",    {nFightCount.min, nFightCount.max}      },
        {"customMacro",   allowCustomMacro                        },
        {"attribute",     json::array()                           },
        {"effects",       json::array()                           },
    };
    for (auto &x : ns_concrete::PlayerTypeMap) {
        j["player"] += {
            {"id",   static_cast<int>(x.first)},
            {"name", x.second                 }
        };
    }
    for (auto &x : PlayerTypeMap) {
        j["attribute"] += {
            {"id",   static_cast<int>(x.first)},
            {"name", x.second                 },
        };
    }
    for (auto &x : ns_concrete::EffectTypeRef) {
        j["effects"] += x;
    }
    return j.dump();
}

std::unique_ptr<DMTask> DMTask::create(const std::string &jsonstr) {
    try {
        nlohmann::json j = nlohmann::json::parse(jsonstr);

        std::unique_ptr<ns_frame::Player> player;
        // player
        switch (static_cast<ns_concrete::PlayerType>(j["player"].get<int>())) {
        case ns_concrete::PlayerType::MjFysj:
            player = ns_concrete::createPlayer(ns_concrete::PlayerType::MjFysj, 0, 0);
            break;
        default:
            throw std::runtime_error("unknown player");
        }
        // attribute
        switch (static_cast<DMTask::AttributeType>(j["attribute"]["id"].get<int>())) {
        case DMTask::AttributeType::jx3box:
            player->attrImportFromJX3BOX(j["attribute"]["data"].get<int>());
            break;
        default:
            throw std::runtime_error("unknown attribute:id");
        }
        // effects
        std::vector<std::shared_ptr<ns_concrete::EffectBase>> effectList;
        for (auto &x : j["effects"].items()) {
            if (x.value().is_boolean() && x.value().get<bool>()) {
                if (ns_concrete::EffectTypeMap.contains(x.key())) {
                    effectList.emplace_back(ns_concrete::createEffect(ns_concrete::EffectTypeMap.at(x.key())));
                } else {
                    throw std::runtime_error("unknown effect");
                }
            }
        }
        std::unique_ptr<DMTask> ret = std::make_unique<DMTask>(DMTask{
            .delayNetwork   = j["delayNetwork"].get<int>(),
            .delayKeyboard  = j["delayKeyboard"].get<int>(),
            .fightTime      = j["fightTime"].get<int>(),
            .fightCount     = j["fightCount"].get<int>(),
            .useCustomMacro = j["customMacro"].is_string(),
            .customMacro    = j["customMacro"].is_string() ? j["customMacro"].get<std::string>() : std::string(),
            .attrBackup     = player->chAttr,
            .effects        = std::move(effectList),
        });
        return ret;
    } catch (nlohmann::json::exception &e) {
        UNREFERENCED_PARAMETER(e);
        CONSTEXPR_LOG_ERROR("DMTask::create: json parse error: {}", e.what());
        return nullptr;
    } catch (std::exception &e) {
        UNREFERENCED_PARAMETER(e);
        CONSTEXPR_LOG_ERROR("DMTask::create: error: {}", e.what());
        return nullptr;
    }
}
