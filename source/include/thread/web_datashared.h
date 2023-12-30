#ifndef THREAD_WEB_SHARED_H_
#define THREAD_WEB_SHARED_H_

#pragma warning(push, 0)
#include <crow.h>
#pragma warning(pop)
#include <mutex>
#include <unordered_map>
namespace ns_thread {

class Coro {};

// 这不是一个线程类. 它被用于保存一些在 WebHandler 和 WebHelper 之间共享的数据.
class WebDataShared {
    std::mutex                                                     mtx;
    std::unordered_map<crow::websocket::connection *, std::string> wsmap;
    std::unordered_map<std::string, Coro>                          coromap;
};

} // namespace ns_thread

#endif // THREAD_WEB_SHARED_H_
