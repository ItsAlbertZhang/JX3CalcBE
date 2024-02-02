#ifndef MODULES_TASK_H_
#define MODULES_TASK_H_

#pragma warning(push, 0)
#ifdef _WIN32
#include <sdkddkver.h>
#endif
#include <crow.h>
#pragma warning(pop)

#include "concrete/effect/base.h"
#include "frame/character/property/attribute.h"
#include "modules/pool.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace ns_modules {

class TaskData {
public:
    // json 数据区
    const int delayNetwork;
    const int delayKeyboard;
    const int fightTime;
    const int fightCount;

    const bool        useCustomMacro = false;
    const std::string customMacro;

    const ns_frame::ChAttr                                      attrBackup;
    const std::vector<std::shared_ptr<ns_concrete::EffectBase>> effects;

    // 根据 json 数据区计算得到的数据区
    const size_t custom_macro_hash = std::hash<std::string>{}(customMacro);
};

class Task {
public:
    Task(const std::string &id, const TaskData &data)
        : id(id), data(data){};
    const std::string             id;
    const TaskData                data;
    std::atomic<bool>             stop{false};
    std::vector<std::future<int>> futures;
};

namespace task {

enum class AttributeType {
    // zero,
    jx3box,
    COUNT,
};
inline std::string AttributeTypeStr[] = {
    // "未启用",
    "从JX3BOX导入",
};
inline const std::unordered_map<std::string, AttributeType> AttributeTypeMap = {
  // {"未启用",       AttributeType::zero  },
    {"从JX3BOX导入", AttributeType::jx3box},
};
nlohmann::json schemaAttribute();

std::string create(const std::string &jsonstr);
void        run(crow::websocket::connection *conn, const std::string &id);
void        stop(crow::websocket::connection *conn);

inline std::unordered_map<std::string, std::unique_ptr<Task>>                   tasksCreated;
inline std::unordered_map<crow::websocket::connection *, std::unique_ptr<Task>> tasksRunning;

inline Pool pool;

inline std::thread       threadIO;
inline asio::io_context  io;
inline std::atomic<bool> iostop{false};

} // namespace task

} // namespace ns_modules

#endif // MODULES_TASK_H_
