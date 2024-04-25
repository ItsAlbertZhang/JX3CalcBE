#include "gdi.h"
#include <memory>

using namespace gdi;

void gdi::tabSelect(Tab tab, select_t &arg) {
    const size_t                                size = 1024 * 1024 * 16;
    static thread_local std::unique_ptr<char[]> data(new char[size]); // 保证在不同表之间不会出现数据竞争
    tabQuery(static_cast<int>(tab), serialize(arg, data.get(), size), size);
    arg = deserialize(data.get(), size);
}
