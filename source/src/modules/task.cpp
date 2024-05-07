#include "modules/task.h"
#include "concrete.h"
#include "frame/character/derived/npc.h"
#include "frame/character/derived/player.h"
#include "frame/character/effect.h"
#include "frame/common/constant.h"
#include "frame/event.h"
#include "frame/global/buff.h"
#include "frame/global/skill.h"
#include "modules/config.h"
#include "plugin/channelinterval.h"
#include "plugin/log.h"
#include <asio/co_spawn.hpp>
#include <chrono>
#include <memory>
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

static void createTaskData(Task::Data &data, Response &res, const std::string &jsonstr) {
    // 1. 验证数据可用性. 此步骤未成功, catch 会返回 config.json not available.
    if (modules::config::dataAvailable == modules::config::dataStatus::unavailable) [[unlikely]]
        throw std::runtime_error("");
    res.next();
    // 2. 解析 json. 此步骤未成功, 返回 json parse error.
    using json   = nlohmann::json;
    const json j = json::parse(jsonstr);
    res.next();

    // 3. basic Data. 此步骤未成功, catch 会返回 Error in base: missing field or invalid option.
    data.playerType    = concrete::playerMap.at(j.at("player").get<std::string>()),
    data.delayNetwork  = j.at("delayNetwork").get<int>(),
    data.delayKeyboard = j.at("delayKeyboard").get<int>(),
    data.fightTime     = j.at("fightTime").get<int>(),
    data.fightCount    = j.at("fightCount").get<int>(),
    res.next();

    // 4. 检查 basic Data. 此步骤未成功, catch 会返回 Error in base: invalid input value.
    if (data.delayNetwork < 0 || data.delayNetwork > modules::config::taskdata::maxDelayNetwork ||
        data.delayKeyboard < 0 || data.delayKeyboard > modules::config::taskdata::maxDelayKeyboard ||
        data.fightTime < 0 || data.fightTime > modules::config::taskdata::maxFightTime ||
        data.fightCount < 0 || data.fightCount > modules::config::taskdata::maxFightCount) [[unlikely]] {
        throw std::runtime_error("");
    }
    res.next();

    // 5. attribute. 此步骤未成功, catch 会返回 Error in attribute.
    auto player   = concrete::createPlayer(data.playerType);
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
    res.next();

    // 6. effect. 此步骤未成功, catch 会返回 Error in effect.
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
    res.next();

    // 7. fight (可选). 此步骤未成功, catch 会返回 Error in fight.
    if (j.contains("fight")) {
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
            data.embedStat = j.at("fight").at("data").get<int>();
        }
    }
    res.next();

    // 8. talents (可选). 此步骤未成功, catch 会返回 Error in talents.
    if (j.contains("talents")) {
        data.talents = j.at("talents").get<frame::Player::typeTalentArray>();
    }
    data.talents = player->getTalents(data.talents);
    res.next();

    // 9. skills (可选). 此步骤未成功, catch 会返回 Error in skills.
    if (j.contains("skills")) {
        for (auto &[key, value] : j.at("skills").items()) {
            data.skills.emplace(
                std::stoi(key),
                frame::Player::Skill{
                    .id      = value.at("id").get<int>(),
                    .level   = value.at("level").get<int>(),
                    .recipes = value.at("recipes").get<std::array<int, CountRecipesPerSkill>>(),
                }
            );
        }
    }
    data.skills = player->getSkills(data.skills);
    res.next();
}

static void asyncStop(Server *self, Task &task) {
    using namespace modules::task;
    self->taskMap.erase(task.id);
}

static auto asyncRun(Server *self, asio::io_context &io, Task &task) -> asio::awaitable<void> {
    using namespace modules::task;

    // 在线程池中创建任务
    const int cntCalcDetail = task.data.fightCount > CNT_DETAIL_TASKS ? CNT_DETAIL_TASKS : task.data.fightCount;
    task.details.resize(cntCalcDetail); // 注意, 此处不是 reserve, 因为获得的内存地址需要传递给线程池
    const Task::Data &data = task.data;
    for (int i = 0; i < cntCalcDetail; i++) {
        self->pool.emplace(task.id, 1, task.futures, calcDetail, data, &task.details[i]);
    }
    self->pool.emplace(task.id, task.data.fightCount - cntCalcDetail, task.futures, calcBrief, data);

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
    asyncStop(self, task);
}

auto modules::task::Server::create(const std::string &jsonstr) -> Response {
    Response    res;
    std::string id = genID(taskMap);
    auto       &it = taskMap.emplace(id, std::make_unique<Task>(id)).first->second;
    try {
        createTaskData(it->data, res, jsonstr);
        asio::co_spawn(ioContext, asyncRun(this, ioContext, *it), asio::detached);
        res.message = std::move(id);
    } catch (const std::exception &e) {
        taskMap.erase(id);
        res.message = e.what();
    }
    return res;
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
    player->embedStat                     = arg.embedStat;
    if (arg.fight.has_value()) {
        player->customLua = frame::CustomLua::get(arg.fight.value());
    }

    std::unique_ptr<frame::NPC> npc = concrete::createNPC(concrete::NPC::NPC124);
    player->targetSelect            = npc.get();

    player->attrImportFromBackup(arg.attrBackup);
    for (auto &it : arg.effects) {
        it->active(player.get());
    }

    frame::Event::clear();
    player->init(arg.skills, arg.talents);
    player->fightStart();
    while (true) {
        if (player->stopInitiative.has_value()) {
            if (player->stopInitiative.value()) [[unlikely]]
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
                name = frame::BuffManager::get(everyDamage.id, everyDamage.level).Name;
            } else {
                name = frame::SkillManager::get(everyDamage.id, everyDamage.level).Name;
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
            j.at("data").emplace_back(objDamage);
        }
    }
    return j.dump();
}
