#ifndef FRAME_TAB_INTERFACE_H_
#define FRAME_TAB_INTERFACE_H_

namespace ns_frame {

class TabString {
public:
    static const char *buffAttrib[];
};

class TabEnum {
public:
    enum class BuffAttrib {
        COUNT,
    };
};

} // namespace ns_frame

#endif // FRAME_TAB_INTERFACE_H_