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
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ns_modules {

namespace task {

enum class AttributeType {
    // zero,
    jx3box,
    COUNT,
};
inline const std::unordered_map<std::string, AttributeType> AttributeTypeMap = {
  // {"未启用",       AttributeType::zero  },
    {"从JX3BOX导入", AttributeType::jx3box},
};

std::string schema();

} // namespace task

class DMTask {
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
    Task(const std::string &id, const DMTask &data)
        : id(id), data(data){};
    const std::string             id;
    const DMTask                  data;
    std::atomic<bool>             stop{false};
    std::vector<std::future<int>> futures;
};

namespace task {
std::unique_ptr<DMTask> create(const std::string &jsonstr);
} // namespace task

} // namespace ns_modules

#endif // MODULES_TASK_H_
