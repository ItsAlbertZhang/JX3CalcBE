#include "modules/task.h"
#include "concrete/character/character.h"
#include "concrete/effect/effect.h"
#include "frame/event.h"
#include "frame/global/uibuff.h"
#include "frame/global/uiskill.h"
#include "plugin/channelinterval.h"
#include "plugin/log.h"
#include "utils/config.h"
#include <asio/co_spawn.hpp>
#include <chrono>
#include <filesystem>
#include <memory>
#include <optional>

#ifdef _WIN32
#include <format>
namespace fmt = std;
#else
#include <fmt/core.h>
#endif

using namespace ns_modules::web::task;

void ns_modules::web::task::server::run() {
    asio::co_spawn(
        io,
        [&]() -> asio::awaitable<void> {
            asio::steady_timer timer(io);
            while (!iostop.load()) {
                timer.expires_after(std::chrono::seconds(1));
                co_await timer.async_wait(asio::use_awaitable);
            }
        },
        asio::detached
    ); // 用于保持 io.run() 的运行
    threadIO = std::thread([&]() {
        io.run();
    });
}

void ns_modules::web::task::server::stop() {
    for (auto &task : taskMap)
        task::stop(task.first);
    iostop.store(true);
    threadIO.join();
}

static int         calcBrief(const Data &arg);
static int         calcDetail(const Data &data, ns_frame::ChDamage *detail);
static std::string genID(int length = 6);

Response ns_modules::web::task::validate(const std::string &jsonstr) {
    using json = nlohmann::json;
    // 解析 json
    json j;
    try {
        j = json::parse(jsonstr);
    } catch (...) {
        return Response{
            .status  = false,
            .content = "json parse error",
        };
    }
    // 检查字段
    // 1. 检查必须字段
    static const std::vector<std::string> required{
        "player", "delayNetwork", "delayKeyboard", "fightTime", "fightCount", "attribute", "effects"
    };
    for (auto &it : required) {
        if (!j.contains(it)) {
            return Response{
                .status  = false,
                .content = fmt::format("missing required field: {}", it),
            };
        }
    }
    // 2. 分别检查字段值
    // 2.1 检查 player
    if (!j["player"].is_string() || !ns_concrete::refPlayerType.contains(j["player"].get<std::string>())) {
        return Response{
            .status  = false,
            .content = "player invalid",
        };
    }
    // 2.2 检查 delayNetwork, delayKeyboard, fightTime, fightCount
    if (!j["delayNetwork"].is_number_integer() || j["delayNetwork"].get<int>() < 0 ||
        j["delayNetwork"].get<int>() > ns_utils::config::taskdata::maxDelayNetwork) {
        return Response{
            .status  = false,
            .content = "delayNetwork invalid",
        };
    }
    if (!j["delayKeyboard"].is_number_integer() || j["delayKeyboard"].get<int>() < 0 ||
        j["delayKeyboard"].get<int>() > ns_utils::config::taskdata::maxDelayKeyboard) {
        return Response{
            .status  = false,
            .content = "delayKeyboard invalid",
        };
    }
    if (!j["fightTime"].is_number_integer() || j["fightTime"].get<int>() < 0 ||
        j["fightTime"].get<int>() > ns_utils::config::taskdata::maxFightTime) {
        return Response{
            .status  = false,
            .content = "fightTime invalid",
        };
    }
    if (!j["fightCount"].is_number_integer() || j["fightCount"].get<int>() < 0 ||
        j["fightCount"].get<int>() > ns_utils::config::taskdata::maxFightCount) {
        return Response{
            .status  = false,
            .content = "fightCount invalid",
        };
    }
    // 2.3 检查 attribute
    if (!j["attribute"].contains("method") || !j["attribute"]["method"].is_string() ||
        !refAttributeType.contains(j["attribute"]["method"].get<std::string>()) ||
        !j["attribute"].contains("data") || !j["attribute"]["data"].is_object()) {
        return Response{
            .status  = false,
            .content = "attribute method invalid",
        };
    }
    AttributeType type = refAttributeType.at(j["attribute"]["method"].get<std::string>());
    switch (type) {
    case AttributeType::jx3box: {
        if (!j["attribute"]["data"].contains("pzid") || !j["attribute"]["data"]["pzid"].is_string()) {
            return Response{
                .status  = false,
                .content = "attribute data invalid",
            };
        }
    } break;
    default:
        return Response{
            .status  = false,
            .content = "attribute data invalid",
        };
        break;
    }
    // 2.4 检查 effects
    if (!j["effects"].is_array()) {
        return Response{
            .status  = false,
            .content = "effects invalid",
        };
    }
    for (auto &it : j["effects"]) {
        if (!it.is_string() || !ns_concrete::refEffectType.contains(it.get<std::string>())) {
            return Response{
                .status  = false,
                .content = fmt::format("effects invalid: {}", it.get<std::string>()),
            };
        }
    }
    // 检查完毕, 返回 true
    return Response{
        .status = true,
    };
}

static std::optional<Data> createTaskData(const nlohmann::json &j) {
    auto playerType = ns_concrete::refPlayerType.at(j["player"].get<std::string>());
    // player. 此处的 player 仅用作属性导入, 因此无需设置 delayNetwork 和 delayKeyboard
    auto player     = ns_concrete::createPlayer(playerType, 0, 0);

    auto attrType = refAttributeType.at(j["attribute"]["method"].get<std::string>());
    switch (attrType) {
    case AttributeType::jx3box: {
        std::string pzid = j["attribute"]["data"]["pzid"].get<std::string>();
        if (!player->attrImportFromJX3BOX(pzid)) {
            return std::nullopt;
        }
    } break;
    default:
        return std::nullopt;
        break;
    }

    std::vector<std::shared_ptr<ns_concrete::EffectBase>> effectList;
    for (auto &x : j["effects"].items()) {
        effectList.emplace_back(ns_concrete::createEffect(ns_concrete::refEffectType.at(x.value().get<std::string>())));
    }

    return Data{
        .delayNetwork   = j["delayNetwork"].get<int>(),
        .delayKeyboard  = j["delayKeyboard"].get<int>(),
        .fightTime      = j["fightTime"].get<int>(),
        .fightCount     = j["fightCount"].get<int>(),
        .useCustomMacro = j.contains("customMacro"),
        .customMacro    = j.contains("customMacro") ? j["customMacro"].get<std::string>() : std::string(),
        .attrBackup     = player->chAttr,
        .effects        = std::move(effectList),
    }; // 返回值会被隐式类型转换为 std::optional
}

static void asyncStop(Task &task) {
    using namespace ns_modules::web::task;
    task.stop.store(true);
    task.futures.clear();
    server::pool.erase(task.id);
    // server::taskMap.erase(task.id);
}

static asio::awaitable<void> asyncRun(asio::io_context &io, Task &task) {
    asio::steady_timer          timer(io);
    const std::chrono::seconds  interval{1};
    const std::filesystem::path p_res = ns_utils::config::pExeDir / "res.tab";
    using namespace ns_modules::web::task;
    // 在线程池中创建任务
    const int cntCalcDetail = task.data.fightCount > 10 ? 10 : task.data.fightCount;
    task.details.resize(cntCalcDetail);
    for (int i = 0; i < cntCalcDetail; i++) {
        server::pool.emplace(task.id, 1, task.futures, calcDetail, task.data, &task.details[i]);
    }
    server::pool.emplace(task.id, task.data.fightCount - cntCalcDetail, task.futures, calcBrief, task.data);
    int cntPre = 0;
    while (task.cntCompleted >= 0 && static_cast<std::vector<int>::size_type>(task.cntCompleted) < task.futures.size() && !task.stop.load()) [[likely]] {
        if (task.futures[task.cntCompleted].wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            {
                std::lock_guard<std::mutex> lock(task.mutex);
                task.results.emplace_back(task.futures[task.cntCompleted].get());
            }
            task.cntCompleted++;
        } else {
            // 先检查是否需要停止
            if (task.stop.load()) [[unlikely]] {
                asyncStop(task);
                co_return;
            }
            {
                std::lock_guard<std::mutex> lock(task.mutex);
                task.speedCurr = task.cntCompleted - cntPre;
            }
            cntPre = task.cntCompleted;
            timer.expires_after(interval);
            co_await timer.async_wait(asio::use_awaitable);
        }
    }
    asyncStop(task);
}

Response ns_modules::web::task::create(const std::string &jsonstr) {
    // 验证 json
    auto res = validate(jsonstr);
    if (!res.status) [[unlikely]]
        return res;
    // 验证后, 使用时无需再次验证
    using json    = nlohmann::json;
    json j        = json::parse(jsonstr);
    auto taskdata = createTaskData(j);
    if (!taskdata.has_value()) [[unlikely]]
        return Response{
            .status  = false,
            .content = "An error occurred while creating task data. Please try again later.",
        };

    auto  id = genID();
    auto &it = server::taskMap.emplace(id, std::make_unique<Task>(id, std::move(taskdata.value()))).first->second;
    asio::co_spawn(server::io, asyncRun(server::io, *it), asio::detached);
    return Response{
        .status  = true,
        .content = id,
    };
}

void ns_modules::web::task::stop(std::string id) {
    if (!server::taskMap.contains(id)) [[unlikely]]
        return;
    server::taskMap.at(id)->stop.store(true); // 真正的停止操作在 asyncRun 中
}

static auto calc(const Data &arg) {
    static thread_local std::unordered_map<size_t, std::unique_ptr<ns_frame::MacroCustom>> map;

    std::unique_ptr<ns_frame::Player> player = ns_concrete::createPlayer(ns_concrete::PlayerType::MjFysj, arg.delayNetwork, arg.delayKeyboard);
    std::unique_ptr<ns_frame::NPC>    npc    = ns_concrete::createNPC(ns_concrete::NPCType::NPC124);
    player->targetSelect                     = npc.get();
    if (arg.useCustomMacro) {
        if (map.find(arg.custom_macro_hash) == map.end()) {
            map.emplace(arg.custom_macro_hash, std::make_unique<ns_frame::MacroCustom>(arg.customMacro));
        }
        player->macroCustom = map.at(arg.custom_macro_hash).get();
    }
    player->attrImportFromBackup(arg.attrBackup);
    for (auto &it : arg.effects) {
        it->active(player.get());
    }
    ns_frame::Event::clear();
    player->macroRun();
    while (ns_frame::Event::now() < static_cast<ns_frame::event_tick_t>(1024 * arg.fightTime)) {
        bool ret = ns_frame::Event::run();
        if (!ret)
            break;
    }

    return player;
}

static int calcBrief(const Data &data) {
    auto player = calc(data);

    unsigned long long sumDamage = 0;
    for (auto &it : player->chDamage) {
        sumDamage += it.damageExcept;
    }

    return static_cast<int>(sumDamage / data.fightTime);
}

static int calcDetail(const Data &data, ns_frame::ChDamage *detail) {
    auto player = calc(data);

    unsigned long long sumDamage = 0;
    for (auto &it : player->chDamage) {
        sumDamage += it.damageExcept;
    }

    *detail = std::move(player->chDamage);
#ifdef D_CONSTEXPR_LOG
    ns_plugin::log::info.save();
    ns_plugin::log::error.save();
#endif
#ifdef D_CONSTEXPR_CHANNELINTERVAL
    ns_plugin::channelinterval::save();
#endif

    return static_cast<int>(sumDamage / data.fightTime);
}

static std::string genID(int length) {
    const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device              random_device;
    std::mt19937                    generator(random_device());
    std::uniform_int_distribution<> distribution(0, static_cast<int>(CHARACTERS.size() - 1));

    std::string random_string;
    while (random_string.empty() || ns_modules::web::task::server::taskMap.contains(random_string)) {
        for (size_t i = 0; i < length; ++i) {
            random_string += CHARACTERS[distribution(generator)];
        }
    }
    return random_string;
}

std::string Task::format() {
    using json = nlohmann::json;
    json j;
    if (cntCompleted == 0) {
        j["status"] = false;
        return j.dump();
    }

    std::lock_guard<std::mutex> lock(mutex);

    unsigned long long damageSum = 0;
    for (int i = 0; i < cntCompleted; i++) {
        damageSum += results[i];
    }
    j["dps"] = damageSum / cntCompleted;

    j["details"] = json::array();
    for (auto &everyFight : details) {
        json objFight = json::array();
        for (auto &everyDamage : everyFight) {
            std::string name;
            switch (everyDamage.source) {
            case ns_frame::DamageSource::skill: {
                const ns_frame::UISkill &skill = ns_frame::UISkillManager::get(everyDamage.id, everyDamage.level);
                name                           = skill.Name;
            } break;
            case ns_frame::DamageSource::buff: {
                const ns_frame::UIBuff &buff = ns_frame::UIBuffManager::get(everyDamage.id, everyDamage.level);
                name                         = buff.Name;
            } break;
            default:
                break;
            }
            json objDamage;
            objDamage["time"]           = everyDamage.tick / 1024.0;
            objDamage["type"]           = static_cast<int>(everyDamage.damageType);
            objDamage["id"]             = everyDamage.id;
            objDamage["level"]          = everyDamage.level;
            objDamage["damageBase"]     = everyDamage.damageBase;
            objDamage["damageCritical"] = everyDamage.damageCritical;
            objDamage["damageExcept"]   = everyDamage.damageExcept;
            objDamage["criticalRate"]   = everyDamage.criticalRate;
            objDamage["name"]           = name;
            objFight.emplace_back(objDamage);
        }
        j["details"].emplace_back(objFight);
    }
    return j.dump();
}
