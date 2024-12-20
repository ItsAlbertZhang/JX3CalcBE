#include "modules/task.h"
#include "concrete.h"
#include "frame/event.h"
#include "frame/global/buff.h"
#include "frame/global/skill.h"
#include "modules/config.h"
#include "plugin/channelinterval.h"
#include "plugin/log.h"
#include "utils/conv.h"
#include <asio/co_spawn.hpp>
#include <chrono>
#include <format>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace jx3calc;
using namespace modules::task;
using ull = unsigned long long;

constexpr int CNT_DETAIL_TASKS = 5;

Server::Server() {
    asio::co_spawn(
        ioContext,
        [this]() -> asio::awaitable<void> {
            asio::steady_timer timer(ioContext);
            while (true) {
                timer.expires_after(std::chrono::seconds(1));
                co_await timer.async_wait(asio::use_awaitable);
            }
        },
        asio::detached
    ); // 用于保持 io.run() 的运行
    ioThread = std::thread([this]() {
        ioContext.run();
    });
}

Server::~Server() {
    for (auto &task : taskMap)
        stop(task.first);
    // 任务结束是异步的 (每秒检查一次), 因此需要等待 1 秒以上. 等待 1.5 秒.
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    ioContext.stop();
    ioThread.join();
    pool.join();
}

Task::Data::~Data() {
    if (fight.has_value()) {
        frame::CustomLua::cancel(fight.value());
    }
}

static int calcBrief(const Task::Data &arg);
static int calcDetail(const Task::Data &data, frame::ChDamage *detail);
template <typename TypeValue>
static std::string genID(const std::unordered_map<std::string, TypeValue> &map, int length = 6);

static Task::Data createTaskData(const std::string &jsonstr) {
    // 1. 验证数据可用性
    if (modules::config::clientType == modules::config::ClientType::unknown) [[unlikely]]
        throw std::runtime_error("服务器数据不可用, 请检查 config.json.");
    // 2. 解析 json
    using json   = nlohmann::json;
    const json j = json::parse(jsonstr); // 解析失败会自动抛出异常

    Task::Data data;

    // 3. 解析基础字段
    try {
        data.playerType = concrete::playerMap.at(j.at("player").get<std::string>());
    } catch (...) {
        throw std::runtime_error("字段非法: player.");
    }
    try {
        data.delayNetwork = j.at("delayNetwork").get<int>();
        if (data.delayNetwork <= 0 || data.delayNetwork > modules::config::taskdata::maxDelayNetwork)
            throw std::runtime_error("");
    } catch (...) {
        throw std::runtime_error("字段非法: delayNetwork.");
    }
    try {
        data.delayKeyboard = j.at("delayKeyboard").get<int>();
        if (data.delayKeyboard <= 0 || data.delayKeyboard > modules::config::taskdata::maxDelayKeyboard)
            throw std::runtime_error("");
    } catch (...) {
        throw std::runtime_error("字段非法: delayKeyboard.");
    }
    try {
        data.fightTime = j.at("fightTime").get<int>();
        if (data.fightTime <= 0 || data.fightTime > modules::config::taskdata::maxFightTime)
            throw std::runtime_error("");
    } catch (...) {
        throw std::runtime_error("字段非法: fightTime.");
    }
    try {
        data.fightCount = j.at("fightCount").get<int>();
        if (data.fightCount <= 0 || data.fightCount > modules::config::taskdata::maxFightCount)
            throw std::runtime_error("");
    } catch (...) {
        throw std::runtime_error("字段非法: fightCount.");
    }

    auto player = concrete::createPlayer(data.playerType);

    // 4. 解析 attribute
    try {
        auto attrType = refAttributeType.at(j.at("attribute")["method"].get<std::string>());
        switch (attrType) {
        case enumAttributeType::data: {
            std::string dataJsonStr = j.at("attribute")["data"].dump();
            if (!player->attrImportFromData(dataJsonStr))
                throw std::runtime_error("import from data failed.");
        } break;
        case enumAttributeType::jx3box: {
            std::string pzid = j.at("attribute")["data"]["pzid"].get<std::string>();
            if (!player->attrImportFromJX3BOX(pzid))
                throw std::runtime_error("import from jx3box failed.");
        } break;
        }
        data.attrBackup = player->chAttr;
    } catch (const std::exception &e) {
        throw std::runtime_error("字段错误: attribute." + std::string(e.what()));
    }

    // 5. 解析 effects
    try {
        std::vector<std::shared_ptr<frame::Effect>> effectList;
        effectList.reserve(j.at("effects").size());
        for (auto &[key, value] : j.at("effects").items()) {
            auto ptr = concrete::createEffect(key, value.dump());
            if (ptr == nullptr) [[unlikely]] {
                throw std::runtime_error("effect error: " + key);
            }
            effectList.emplace_back(std::move(ptr));
        }
        data.effects = std::move(effectList);
    } catch (const std::exception &e) {
        throw std::runtime_error("字段错误: effects." + std::string(e.what()));
    }

    // 6. 解析 fight (可选)
    if (j.contains("fight")) {
        try {
            const json &fight = j.at("fight");
            if (fight.contains("method") && refCustom.contains(fight.at("method").get<std::string>())) {
                switch (refCustom.at(fight.at("method").get<std::string>())) {
                case enumCustom::lua:
                    if (!modules::config::taskdata::allowCustom) [[unlikely]]
                        throw std::runtime_error("custom fight is not allowed.");
                    // fight["data"] 为 string, 内容为 lua 代码
                    data.fight.emplace(fight.at("data").get<std::string>());
                    break;
                case enumCustom::jx3:
                    // fight["data"] 为 string[], 每一项都是一个游戏内宏
                    std::vector<std::string> v;
                    for (auto &it : fight.at("data")) {
                        v.emplace_back(it.get<std::string>());
                    }
                    data.fight.emplace(frame::CustomLua::parse(v));
                    break;
                }

            } else if (fight.contains("data") && fight.at("data").is_number_integer()) {
                player->fightType = j.at("fight").at("data").get<int>();
                data.fightType    = player->fightType;
            }
        } catch (const std::exception &e) {
            throw std::runtime_error("字段错误: fight." + std::string(e.what()));
        }
    }

    // 7. 解析 talents (可选)
    if (j.contains("talents")) {
        try {
            data.talents = j.at("talents").get<frame::Player::typeTalents>();
        } catch (...) {
            throw std::runtime_error("字段错误: talents.");
        }
    }
    // 8. 解析 recipes (可选)
    if (j.contains("recipes")) {
        try {
            for (auto &[key, value] : j.at("recipes").items()) {
                int skillID           = std::stoi(key);
                data.recipes[skillID] = value.get<frame::Player::typeRecipe>();
            }
        } catch (...) {
            throw std::runtime_error("字段错误: recipes.");
        }
    }
    player->initValidate(data.skills, data.talents, data.recipes);

    return data;
}

static void asyncStop(Server *self, Task &task) {
    using namespace modules::task;
    self->taskMap.erase(task.id);
}

// int calcDetail(const Data &data, frame::ChDamage *detail)
using t1Arg1 = const Task::Data &;
using t1Arg2 = std::vector<frame::Damage> *;
using t1Func = Server::tPoolRet (&)(t1Arg1, t1Arg2);

template void modules::Pool<Server::tPoolRet>::emplace<t1Func, t1Arg1, t1Arg2>(
    const std::string &,
    int,
    std::vector<std::future<Server::tPoolRet>> &,
    t1Func,
    t1Arg1 &&,
    t1Arg2 &&
);

// int calcBrief(const Data &arg)
using t2Arg1 = const Task::Data &;
using t2Func = Server::tPoolRet (&)(t2Arg1);

template void modules::Pool<Server::tPoolRet>::emplace<t2Func, t2Arg1>(
    const std::string &,
    int,
    std::vector<std::future<Server::tPoolRet>> &,
    t2Func,
    t2Arg1 &&
);

static auto asyncRun(Server *self, asio::io_context &io, Task &task) -> asio::awaitable<void> {
    using namespace modules::task;

    // 在线程池中创建任务
    const int cntCalcDetail = task.data.fightCount > CNT_DETAIL_TASKS ? CNT_DETAIL_TASKS : task.data.fightCount;
    task.details.resize(cntCalcDetail); // 注意, 此处不是 reserve, 因为获得的内存地址需要传递给线程池
    const Task::Data &data = task.data;
    for (int i = 0; i < cntCalcDetail; i++)
        self->pool.emplace(task.id, 1, task.futures, calcDetail, data, &task.details[i]);
    self->pool.emplace(task.id, task.data.fightCount - cntCalcDetail, task.futures, calcBrief, data);

    task.results.reserve(task.data.fightCount);
    int  cntPre    = 0;
    auto timeStart = std::chrono::steady_clock::now();
    while (!task.stop.load() && task.cntCompleted < task.data.fightCount) [[likely]] {
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - timeStart).count();
        if (duration > modules::config::taskdata::maxTaskDuration) {
            self->pool.erase(task.id);
            task.stop.store(true);
            break;
        }
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
            timer.expires_after(std::chrono::seconds {1});
            co_await timer.async_wait(asio::use_awaitable);
        }
    }
    if (!task.stop.load()) { // 正常结束
        asio::steady_timer timer(io);
        timer.expires_after(std::chrono::seconds {60}); // 等待 60 秒, 随后释放内存
        co_await timer.async_wait(asio::use_awaitable);
    }
    asyncStop(self, task);
}

auto modules::task::Server::create(const std::string &jsonstr) -> std::string {
    std::string id = genID(taskMap);

    try {
        auto &it = taskMap.emplace(id, std::make_unique<Task>(id, createTaskData(jsonstr))).first->second;
        asio::co_spawn(ioContext, asyncRun(this, ioContext, *it), asio::detached);
        return std::format("{{\"status\":0,\"data\":\"{}\"}}", id);
    } catch (const std::exception &e) {
        taskMap.erase(id);
        return std::format("{{\"status\":-1,\"data\":\"{}\"}}", e.what());
    }
}

void modules::task::Server::stop(std::string id) {
    if (!taskMap.contains(id)) [[unlikely]]
        return;
    taskMap.at(id)->stop.store(true); // 真正的停止操作在 asyncRun 中
}

static auto calc(const Task::Data &arg) -> std::unique_ptr<frame::Player> {
    std::unique_ptr<frame::Player> player = concrete::createPlayer(arg.playerType);
    player->delayBase                     = arg.delayNetwork;
    player->delayRand                     = arg.delayKeyboard;
    player->fightType                     = arg.fightType;
    player->fightTick                     = arg.fightTime * 1024;
    if (arg.fight.has_value()) {
        player->customLua = frame::CustomLua::get(arg.fight.value());
    }

    std::unique_ptr<frame::NPC> npc = concrete::createNPC(concrete::NPCType::NPCatLevelAdd4);
    player->targetSelect            = npc.get();
    frame::Event::clear();

    player->attrImportFromBackup(arg.attrBackup);
    for (auto &it : arg.effects) {
        it->active(player.get());
    }

    player->init(arg.skills, arg.talents, arg.recipes);
    player->fightStart();
    while (true) {
        if (player->fightStopWait.has_value()) {
            if (player->fightStopWait.value() == 0) [[unlikely]]
                break;
        } else if (frame::Event::now() >= static_cast<frame::event_tick_t>(arg.fightTime) * 1024) [[unlikely]]
            break;
        bool ret = frame::Event::run();
        if (!ret)
            break;
    }

    return player;
}

static int calcBrief(const Task::Data &data) {
    auto player = calc(data);

    ull sumDamage = 0;
    for (auto &it : player->chDamage) {
        sumDamage += it.damageExcept;
    }

    return static_cast<int>(sumDamage * 1024 / frame::Event::now());
}

static int calcDetail(const Task::Data &data, frame::ChDamage *detail) {
    auto player = calc(data);

    ull sumDamage = 0;
    for (auto &it : player->chDamage) {
        sumDamage += it.damageExcept;
    }

    *detail = std::move(player->chDamage);
#ifdef D_CONSTEXPR_LOG
    plugin::log::info.save();
    plugin::log::info.enable = false;
    plugin::log::error.save();
#endif
#ifdef D_CONSTEXPR_CHANNELINTERVAL
    plugin::channelinterval::save();
#endif

    return static_cast<int>(sumDamage * 1024 / frame::Event::now());
}

template <typename TValue>
static std::string genID(const std::unordered_map<std::string, TValue> &map, int length) {
    const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device              random_device;
    std::mt19937                    generator(random_device());
    std::uniform_int_distribution<> distribution(0, static_cast<int>(CHARACTERS.size() - 1));

    std::string random_string;
    while (random_string.empty() || map.contains(random_string)) {
        for (int i = 0; i < length; i++) {
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
                name = frame::BuffManager::get(everyDamage.id, everyDamage.level).Name;
            } else {
                name = frame::SkillManager::get(everyDamage.id, everyDamage.level).Name;
            }
            json objDamage;
            objDamage["time"]           = everyDamage.tick / 1024.0;
            objDamage["type"]           = everyDamage.damageType;
            objDamage["id"]             = everyDamage.id;
            objDamage["level"]          = everyDamage.level;
            objDamage["damageBase"]     = everyDamage.damageBase;
            objDamage["damageCritical"] = everyDamage.damageCritical;
            objDamage["damageExcept"]   = everyDamage.damageExcept;
            objDamage["criticalRate"]   = everyDamage.criticalRate;
            objDamage["isCritical"]     = everyDamage.isCritical;
            objDamage["name"]           = config::isUTF8 ? name : utils::gbk2utf8(name);
            objFight.emplace_back(objDamage);
        }
        j.at("data").emplace_back(objFight);
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
        int         count;
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
                    name = frame::BuffManager::get(everyDamage.id, everyDamage.level).Name;
                } else {
                    name = frame::SkillManager::get(everyDamage.id, everyDamage.level).Name;
                }
                damageAnalysisMap[everyDamage.id][everyDamage.level] = DamageAnalysisItem {
                    .id        = everyDamage.id,
                    .level     = everyDamage.level,
                    .name      = name,
                    .count     = 1,
                    .damageMin = everyDamage.damageBase,
                    .damageMax = everyDamage.damageCritical,
                    .damageSum = everyDamage.damageExcept,
                };
            } else {
                damageAnalysisMap[everyDamage.id][everyDamage.level].count += 1;
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
            objDamage["name"]       = config::isUTF8 ? it.name : utils::gbk2utf8(it.name);
            objDamage["count"]      = static_cast<double>(it.count) / CNT_DETAIL_TASKS;
            objDamage["damageMin"]  = it.damageMin;
            objDamage["damageMax"]  = it.damageMax;
            objDamage["proportion"] = static_cast<double>(it.damageSum) / sumDamage;
            j.at("data").emplace_back(objDamage);
        }
    }
    return j.dump();
}
