#ifndef THREAD_DATA_MODELS_P_ARG_H_
#define THREAD_DATA_MODELS_P_ARG_H_

#include "frame/character/property/attribute.h"
#include <memory>
#include <string>

namespace ns_thread {

class DMTask {
public:
    const ns_frame::ChAttr attrBackup;

    const int delayNetwork = 45;
    const int delayKeybord = 20;
    const int fightTime    = 300;
    const int fightCount   = 100;

    const bool tz_jn = false;
    const bool tz_tx = false;
    const bool dfm_y = false;
    const bool dfm_w = false;
    const bool dfm_x = false;
    const bool wq_cw = false;

    const bool        useCustomMacro = false;
    const std::string customMacro;
    const size_t      custom_macro_hash = std::hash<std::string>{}(customMacro);

    class intValue {
    public:
        int min;
        int max;
    };

    static inline intValue nDelayNetwork{0, 1024};
    static inline intValue nDelayKeybord{0, 1024};
    static inline intValue nFightTime{1, 1 << 30};
    static inline intValue nFightCount{1, 1 << 30};

    enum class AttributeType {
        zero,
        jx3box,
        COUNT,
    };
    static inline const char *PlayerTypeName[] = {
        "未启用",
        "从JX3BOX导入",
    };

    static std::string             getTemplate();
    static std::unique_ptr<DMTask> createInstance(const std::string &jsonstr);
};

} // namespace ns_thread

#endif // THREAD_DATA_MODELS_P_ARG_H_
