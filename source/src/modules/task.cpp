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
#include <memory>
#include <optional>
#include <unordered_map>

#ifdef _WIN32
#include <format>
namespace fmt = std;
#else
#include <fmt/core.h>
#endif

using namespace ns_modules::task;
using ull = unsigned long long;

constexpr int CNT_DETAIL_TASKS = 10;

void ns_modules::task::server::asyncrun() {
    asio::co_spawn(
        io,
        [&]() -> asio::awaitable<void> {
            asio::steady_timer timer(io);
            while (true) {
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

void ns_modules::task::server::stop() {
    for (auto &task : taskMap)
        task::stop(task.first);
    io.stop();
    threadIO.join();
}

static int         calcBrief(const Data &arg);
static int         calcDetail(const Data &data, ns_frame::ChDamage *detail);
static std::string genID(int length = 6);

Response ns_modules::task::validate(const std::string &jsonstr) {
    using json = nlohmann::json;
    // 解析 json
    json j;
    try {
        j = json::parse(jsonstr);
    } catch (...) {
        return Response{
            .status = ResponseStatus::parse_error,
            .data   = "json parse error",
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
                .status = ResponseStatus::missing_field,
                .data   = fmt::format("missing required field: {}", it),
            };
        }
    }
    // 2. 分别检查字段值
    // 2.1 检查 player
    if (!j["player"].is_string() || !ns_concrete::refPlayerType.contains(j["player"].get<std::string>())) {
        return Response{
            .status = ResponseStatus::invalid_player,
            .data   = "player invalid",
        };
    }
    // 2.2 检查 delayNetwork, delayKeyboard, fightTime, fightCount
    if (!j["delayNetwork"].is_number_integer() || j["delayNetwork"].get<int>() < 0 ||
        j["delayNetwork"].get<int>() > ns_utils::config::taskdata::maxDelayNetwork) {
        return Response{
            .status = ResponseStatus::invalid_interger,
            .data   = "delayNetwork invalid",
        };
    }
    if (!j["delayKeyboard"].is_number_integer() || j["delayKeyboard"].get<int>() < 0 ||
        j["delayKeyboard"].get<int>() > ns_utils::config::taskdata::maxDelayKeyboard) {
        return Response{
            .status = ResponseStatus::invalid_interger,
            .data   = "delayKeyboard invalid",
        };
    }
    if (!j["fightTime"].is_number_integer() || j["fightTime"].get<int>() < 0 ||
        j["fightTime"].get<int>() > ns_utils::config::taskdata::maxFightTime) {
        return Response{
            .status = ResponseStatus::invalid_interger,
            .data   = "fightTime invalid",
        };
    }
    if (!j["fightCount"].is_number_integer() || j["fightCount"].get<int>() < 0 ||
        j["fightCount"].get<int>() > ns_utils::config::taskdata::maxFightCount) {
        return Response{
            .status = ResponseStatus::invalid_interger,
            .data   = "fightCount invalid",
        };
    }
    // 2.3 检查 attribute
    if (!j["attribute"].contains("method") || !j["attribute"]["method"].is_string() ||
        !refAttributeType.contains(j["attribute"]["method"].get<std::string>()) ||
        !j["attribute"].contains("data") || !j["attribute"]["data"].is_object()) {
        return Response{
            .status = ResponseStatus::invalid_attribute_method,
            .data   = "attribute method invalid",
        };
    }
    AttributeType type = refAttributeType.at(j["attribute"]["method"].get<std::string>());
    switch (type) {
    case AttributeType::data:
        break;
    case AttributeType::jx3box: {
        if (!j["attribute"]["data"].contains("pzid") || !j["attribute"]["data"]["pzid"].is_string()) {
            return Response{
                .status = ResponseStatus::invalid_attribute_data,
                .data   = "attribute data invalid",
            };
        }
    } break;
    default:
        return Response{
            .status = ResponseStatus::invalid_attribute_method,
            .data   = "attribute method invalid",
        };
        break;
    }
    // 2.4 检查 effects
    if (!j["effects"].is_array()) {
        return Response{
            .status = ResponseStatus::invalid_effects,
            .data   = "effects invalid",
        };
    }
    for (auto &it : j["effects"]) {
        if (!it.is_string() || !ns_concrete::refEffectType.contains(it.get<std::string>())) {
            return Response{
                .status = ResponseStatus::invalid_effects,
                .data   = fmt::format("effects invalid: {}", it.get<std::string>()),
            };
        }
    }
    // 检查完毕, 返回 true
    return Response{
        .status = ResponseStatus::success,
    };
}

static std::optional<Data> createTaskData(const nlohmann::json &j) {
    auto playerType = ns_concrete::refPlayerType.at(j["player"].get<std::string>());
    // player. 此处的 player 仅用作属性导入, 因此无需设置 delayNetwork 和 delayKeyboard
    auto player     = ns_concrete::createPlayer(playerType, 0, 0);

    auto attrType = refAttributeType.at(j["attribute"]["method"].get<std::string>());
    switch (attrType) {
    case AttributeType::data: {
        std::string dataJsonStr = j["attribute"]["data"].dump();
        if (!player->attrImportFromData(dataJsonStr)) {
            return std::nullopt;
        }
    } break;
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
    using namespace ns_modules::task;
    task.futures.clear();
    server::pool.erase(task.id);
    server::taskMap.erase(task.id);
}

static asio::awaitable<void> asyncRun(asio::io_context &io, Task &task) {
    using namespace ns_modules::task;

    // 在线程池中创建任务
    const int cntCalcDetail = task.data.fightCount > CNT_DETAIL_TASKS ? CNT_DETAIL_TASKS : task.data.fightCount;
    task.details.resize(cntCalcDetail);
    for (int i = 0; i < cntCalcDetail; i++) {
        server::pool.emplace(task.id, 1, task.futures, calcDetail, task.data, &task.details[i]);
    }
    server::pool.emplace(task.id, task.data.fightCount - cntCalcDetail, task.futures, calcBrief, task.data);

    task.results.reserve(task.data.fightCount);
    task.details.reserve(cntCalcDetail);
    int cntPre = 0;
    while (!task.stop.load() && task.cntCompleted < task.data.fightCount) [[likely]] {
        if (task.futures[task.cntCompleted].wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            {
                std::lock_guard<std::mutex> lock(task.mutex);
                task.results.emplace_back(task.futures[task.cntCompleted].get());
                task.cntCompleted++;
            }
        } else {
            {
                std::lock_guard<std::mutex> lock(task.mutex);
                task.speedCurr = task.cntCompleted - cntPre;
            }
            cntPre = task.cntCompleted;
            asio::steady_timer timer(io);
            timer.expires_after(std::chrono::seconds{1});
            co_await timer.async_wait(asio::use_awaitable);
        }
    }
    if (!task.stop.load()) { // 正常结束
        asio::steady_timer timer(io);
        timer.expires_after(std::chrono::seconds{60}); // 等待 60 秒, 随后释放内存
        co_await timer.async_wait(asio::use_awaitable);
    }
    asyncStop(task);
}

Response ns_modules::task::create(const std::string &jsonstr) {
    // 验证数据可用性
    if (!ns_utils::config::dataAvailable) [[unlikely]]
        return Response{
            .status = ResponseStatus::config_error,
            .data   = "Data not available. Please config.",
        };
    // 验证 json
    auto res = validate(jsonstr);
    if (res.status != ResponseStatus::success) [[unlikely]]
        return res;
    // 验证后, 使用时无需再次验证
    using json    = nlohmann::json;
    json j        = json::parse(jsonstr);
    auto taskdata = createTaskData(j);
    if (!taskdata.has_value()) [[unlikely]]
        return Response{
            .status = ResponseStatus::create_data_error,
            .data   = "An error occurred while creating task data. Please try again later.",
        };

    auto  id = genID();
    auto &it = server::taskMap.emplace(id, std::make_unique<Task>(id, std::move(taskdata.value()))).first->second;
    asio::co_spawn(server::io, asyncRun(server::io, *it), asio::detached);
    return Response{
        .status = ResponseStatus::success,
        .data   = id,
    };
}

void ns_modules::task::stop(std::string id) {
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

    ull sumDamage = 0;
    for (auto &it : player->chDamage) {
        sumDamage += it.damageExcept;
    }

    return static_cast<int>(sumDamage / data.fightTime);
}

static int calcDetail(const Data &data, ns_frame::ChDamage *detail) {
    auto player = calc(data);

    ull sumDamage = 0;
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
    while (random_string.empty() || ns_modules::task::server::taskMap.contains(random_string)) {
        for (size_t i = 0; i < length; ++i) {
            random_string += CHARACTERS[distribution(generator)];
        }
    }
    return random_string;
}

using json = nlohmann::json;

std::string Task::queryDPS() {
    json j;
    if (cntCompleted == 0) [[unlikely]] {
        j["status"] = 1; // 无数据
        j["data"]   = "No data available.";
        return j.dump();
    }

    std::lock_guard<std::mutex> lock(mutex);

    j["status"] = 0; // 成功
    j["data"]   = json::object();
    if (cntCompleted < data.fightCount) [[likely]] {
        j["data"]["complete"] = false;
    } else {
        j["data"]["complete"] = true;
    }

    j["data"]["list"] = json::array();
    ull avg           = 0; // 平均值
    for (int i = 0; i < cntCompleted; i++) {
        j["data"]["list"].emplace_back(results[i]);
        avg += results[i];
    }
    avg /= cntCompleted;
    j["data"]["avg"] = avg;

    ull sd = 0; // 标准差
    for (int i = 0; i < cntCompleted; i++) {
        sd += (results[i] - avg) * (results[i] - avg);
    }
    sd              = static_cast<ull>(std::sqrt(sd / cntCompleted));
    j["data"]["sd"] = sd;

    j["data"]["speed"]   = speedCurr;
    j["data"]["current"] = cntCompleted;
    j["data"]["total"]   = data.fightCount;

    return j.dump();
}

std::string Task::queryDamageList() {
    json j;
    if (cntCompleted < CNT_DETAIL_TASKS && cntCompleted < data.fightCount) [[unlikely]] {
        j["status"] = 1; // 数据不足
        j["data"]   = "Data not available. Try again later.";
        return j.dump();
    }

    std::lock_guard<std::mutex> lock(mutex);

    j["status"] = 0; // 成功
    j["data"]   = json::array();
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
        j["data"].emplace_back(objFight);
    }
    return j.dump();
}

std::string Task::queryDamageAnalysis() {
    json j;
    if (cntCompleted < CNT_DETAIL_TASKS && cntCompleted < data.fightCount) [[unlikely]] {
        j["status"] = 1; // 数据不足
        j["data"]   = "Data not available. Try again later.";
        return j.dump();
    }

    std::lock_guard<std::mutex> lock(mutex);

    j["status"] = 0; // 成功
    j["data"]   = json::array();

    class DamageAnalysisItem {
    public:
        int         id;
        int         level;
        std::string name;
        int         damageMin;
        int         damageMax;
        int         damageSum;
    };
    std::unordered_map<int, std::unordered_map<int, DamageAnalysisItem>> damageAnalysisMap;
    ull                                                                  sumDamage = 0;
    for (auto &everyFight : details) {
        for (auto &everyDamage : everyFight) {
            if (!damageAnalysisMap.contains(everyDamage.id) || !damageAnalysisMap.at(everyDamage.id).contains(everyDamage.level)) {
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
                damageAnalysisMap[everyDamage.id][everyDamage.level] = DamageAnalysisItem{
                    .id        = everyDamage.id,
                    .level     = everyDamage.level,
                    .name      = name,
                    .damageMin = everyDamage.damageBase,
                    .damageMax = everyDamage.damageCritical,
                    .damageSum = everyDamage.damageExcept,
                };
            } else {
                damageAnalysisMap[everyDamage.id][everyDamage.level].damageMin = std::min(damageAnalysisMap[everyDamage.id][everyDamage.level].damageMin, everyDamage.damageBase);
                damageAnalysisMap[everyDamage.id][everyDamage.level].damageMax = std::max(damageAnalysisMap[everyDamage.id][everyDamage.level].damageMax, everyDamage.damageCritical);
                damageAnalysisMap[everyDamage.id][everyDamage.level].damageSum += everyDamage.damageExcept;
            }
            sumDamage += everyDamage.damageExcept;
        }
    }
    for (auto &it1 : damageAnalysisMap) {
        for (auto &it2 : it1.second) {
            auto &it = it2.second;
            json  objDamage;
            objDamage["id"]         = it.id;
            objDamage["level"]      = it.level;
            objDamage["name"]       = it.name;
            objDamage["damageMin"]  = it.damageMin;
            objDamage["damageMax"]  = it.damageMax;
            objDamage["proportion"] = static_cast<double>(it.damageSum) / sumDamage;
            j["data"].emplace_back(objDamage);
        }
    }
    return j.dump();
}
