#include "modules/task.h"
#include "concrete/character/character.h"
#include "concrete/effect/effect.h"
#include "frame/character/derived/player.h"
#include "frame/event.h"
#include "frame/global/buff.h"
#include "frame/global/skill.h"
#include "plugin/channelinterval.h"
#include "plugin/log.h"
#include "utils/config.h"
#include <asio/co_spawn.hpp>
#include <chrono>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

using namespace ns_modules::task;
using ull = unsigned long long;

constexpr int CNT_DETAIL_TASKS = 5;

void ns_modules::task::server::asyncrun() {
    asio::co_spawn(
        ioContext,
        [&]() -> asio::awaitable<void> {
            asio::steady_timer timer(ioContext);
            while (true) {
                timer.expires_after(std::chrono::seconds(1));
                co_await timer.async_wait(asio::use_awaitable);
            }
        },
        asio::detached
    ); // 用于保持 io.run() 的运行
    ioThread = std::thread([&]() {
        ioContext.run();
    });
}

void ns_modules::task::server::stop() {
    for (auto &task : taskMap)
        task::stop(task.first);
    ioContext.stop();
    ioThread.join();
    pool.join();
}

ns_modules::task::Data::~Data() {
    if (!customString.empty()) {
        ns_frame::CustomLua::cancel(customString);
    }
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
    // 1.1 检查必须字段
    static const std::vector<std::string> required{
        "player", "delayNetwork", "delayKeyboard", "fightTime", "fightCount", "attribute", "effects"
    };
    for (auto &it : required) {
        if (!j.contains(it)) {
            return Response{
                .status = ResponseStatus::missing_field,
                .data   = std::format("missing required field: {}", it),
            };
        }
    }
    // 1.2 检查不允许字段
    if (j.contains("custom") && !ns_utils::config::taskdata::allowCustom) {
        return Response{
            .status = ResponseStatus::invalid_field,
            .data   = "custom not allowed",
        };
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
    if (!j["attribute"].is_object() ||
        !j["attribute"].contains("method") || !j["attribute"]["method"].is_string() ||
        !refAttributeType.contains(j["attribute"]["method"].get<std::string>()) ||
        !j["attribute"].contains("data") || !j["attribute"]["data"].is_object()) {
        return Response{
            .status = ResponseStatus::invalid_attribute_method,
            .data   = "attribute method invalid",
        };
    }
    enumAttributeType type = refAttributeType.at(j["attribute"]["method"].get<std::string>());
    switch (type) {
    case enumAttributeType::data:
        break;
    case enumAttributeType::jx3box: {
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
                .data   = std::format("effects invalid: {}", it.get<std::string>()),
            };
        }
    }
    // 2.5 检查 custom
    if (j.contains("custom")) {
        bool invalid = false;
        if (!j["custom"].is_object() ||
            !j["custom"].contains("method") || !j["custom"]["method"].is_string() ||
            !refCustom.contains(j["custom"]["method"].get<std::string>()) ||
            !j["custom"].contains("data")) {
            invalid = true;
        }
        switch (refCustom.at(j["custom"]["method"].get<std::string>())) {
        case enumCustom::lua:
            if (!j["custom"]["data"].is_string()) {
                invalid = true;
            }
            break;
        case enumCustom::jx3:
            if (!j["custom"]["data"].is_array()) {
                invalid = true;
            } else {
                for (auto &it : j["custom"]["data"]) {
                    if (!it.is_string()) {
                        invalid = true;
                        break;
                    }
                }
            }
            break;
        }
        if (invalid) {
            return Response{
                .status = ResponseStatus::invalid_custom,
                .data   = "custom method invalid",
            };
        }
    }
    // 检查完毕, 返回 true
    return Response{
        .status = ResponseStatus::success,
    };
}

static std::optional<Data> createTaskData(const nlohmann::json &j) {
    Data res{
        .delayNetwork  = j["delayNetwork"].get<int>(),
        .delayKeyboard = j["delayKeyboard"].get<int>(),
        .fightTime     = j["fightTime"].get<int>(),
        .fightCount    = j["fightCount"].get<int>(),
    };

    auto playerType = ns_concrete::refPlayerType.at(j["player"].get<std::string>());
    // player. 此处的 player 仅用作属性导入, 因此无需设置 delayNetwork 和 delayKeyboard
    auto player     = ns_concrete::createPlayer(playerType, 0, 0);

    auto attrType = refAttributeType.at(j["attribute"]["method"].get<std::string>());
    switch (attrType) {
    case enumAttributeType::data: {
        std::string dataJsonStr = j["attribute"]["data"].dump();
        if (!player->attrImportFromData(dataJsonStr)) {
            return std::nullopt;
        }
    } break;
    case enumAttributeType::jx3box: {
        std::string pzid = j["attribute"]["data"]["pzid"].get<std::string>();
        if (!player->attrImportFromJX3BOX(pzid)) {
            return std::nullopt;
        }
    } break;
    default:
        return std::nullopt;
        break;
    }
    res.attrBackup = player->chAttr;

    std::vector<std::shared_ptr<ns_concrete::EffectBase>> effectList;
    for (auto &x : j["effects"].items()) {
        auto effect = ns_concrete::createEffect(ns_concrete::refEffectType.at(x.value().get<std::string>()));
        if (effect == nullptr) [[unlikely]] {
            return std::nullopt; // 代码未出错的情况下, 不可能出现此情况, 因为 validate 已经验证过了
        }
        effectList.emplace_back(std::move(effect));
    }
    res.effects = std::move(effectList);

    if (j.contains("custom")) {
        switch (refCustom.at(j["custom"]["method"].get<std::string>())) {
        case enumCustom::lua:
            res.customString = j["custom"]["data"].get<std::string>();
            break;
        case enumCustom::jx3:
            std::vector<std::string> v;
            for (auto &it : j["custom"]["data"]) {
                v.emplace_back(it.get<std::string>());
            }
            res.customString = ns_frame::CustomLua::parse(v);
            break;
        }
    }

    return res;
}

static void asyncStop(Task &task) {
    using namespace ns_modules::task;
    server::taskMap.erase(task.id);
}

static asio::awaitable<void> asyncRun(asio::io_context &io, Task &task) {
    using namespace ns_modules::task;

    // 在线程池中创建任务
    const int cntCalcDetail = task.data.fightCount > CNT_DETAIL_TASKS ? CNT_DETAIL_TASKS : task.data.fightCount;
    task.details.resize(cntCalcDetail); // 注意, 此处不是 reserve, 因为获得的内存地址需要传递给线程池
    for (int i = 0; i < cntCalcDetail; i++) {
        server::pool.emplace(task.id, 1, task.futures, calcDetail, task.data, &task.details[i]);
    }
    server::pool.emplace(task.id, task.data.fightCount - cntCalcDetail, task.futures, calcBrief, task.data);

    task.results.reserve(task.data.fightCount);
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
    asio::co_spawn(server::ioContext, asyncRun(server::ioContext, *it), asio::detached);
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
    std::unique_ptr<ns_frame::Player> player = ns_concrete::createPlayer(ns_concrete::PlayerType::MjFysj, arg.delayNetwork, arg.delayKeyboard);
    std::unique_ptr<ns_frame::NPC>    npc    = ns_concrete::createNPC(ns_concrete::NPCType::NPC124);
    player->targetSelect                     = npc.get();
    if (!arg.customString.empty()) {
        player->customLua = ns_frame::CustomLua::get(arg.customString);
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
        for (size_t i = 0; i < length; i++) {
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

    std::lock_guard<std::mutex> lock(mutex); // 上锁

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

    ull sd  = 0; // 标准差
    int min = static_cast<int>(avg);
    int max = static_cast<int>(avg);
    int md  = 0;
    for (int i = 0; i < cntCompleted; i++) {
        sd += (results[i] - avg) * (results[i] - avg);
        if (results[i] < min) {
            min = results[i];
            md  = static_cast<int>(avg - min);
        } else if (results[i] > max) {
            max = results[i];
            md  = static_cast<int>(max - avg);
        }
    }
    sd = static_cast<ull>(std::sqrt(sd / cntCompleted));

    const double z99  = 2.576;
    int          ci99 = static_cast<int>(z99 * sd / std::sqrt(static_cast<double>(cntCompleted)));

    j["data"]["sd"]   = sd;
    j["data"]["min"]  = min;
    j["data"]["max"]  = max;
    j["data"]["md"]   = md;
    j["data"]["ci99"] = ci99;

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
            if (everyDamage.isBuff) {
                name = ns_frame::BuffManager::get(everyDamage.id, everyDamage.level).Name;
            } else {
                name = ns_frame::SkillManager::get(everyDamage.id, everyDamage.level).Name;
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
            objDamage["isCritical"]     = everyDamage.isCritical;
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
                if (everyDamage.isBuff) {
                    name = ns_frame::BuffManager::get(everyDamage.id, everyDamage.level).Name;
                } else {
                    name = ns_frame::SkillManager::get(everyDamage.id, everyDamage.level).Name;
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
