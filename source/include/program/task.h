#ifndef PROGRAM_TASK_H_
#define PROGRAM_TASK_H_

#include "concrete/effects/base.h"
#include "frame/character/property/attribute.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ns_program {

class DMTask {
public:
    // 静态数据区: 声明及设置无关变量
    class intValue {
    public:
        int min;
        int max;
    };

    enum class AttributeType {
        zero,
        jx3box,
        COUNT,
    };
    static inline std::unordered_map<AttributeType, std::string> PlayerTypeMap = {
        {AttributeType::zero,   "未启用"      },
        {AttributeType::jx3box, "从JX3BOX导入"},
    };

    // 静态数据区: 设置相关变量
    static inline intValue nDelayNetwork{0, 1024};
    static inline intValue nDelayKeybord{0, 1024};
    static inline intValue nFightTime{1, 1 << 30};
    static inline intValue nFightCount{1, 1 << 30};

    static inline bool allowCustomMacro = true;

    // 静态方法区
    static std::string             format();
    static std::unique_ptr<DMTask> create(const std::string &jsonstr);

    // json 数据区
    const int delayNetwork = 45;
    const int delayKeybord = 20;
    const int fightTime    = 300;
    const int fightCount   = 100;

    const bool        useCustomMacro = false;
    const std::string customMacro;

    const ns_frame::ChAttr                                      attrBackup;
    const std::vector<std::shared_ptr<ns_concrete::EffectBase>> effects;

    // 根据 json 数据区计算得到的数据区
    const size_t custom_macro_hash = std::hash<std::string>{}(customMacro);
};

} // namespace ns_program

#endif // PROGRAM_TASK_H_
