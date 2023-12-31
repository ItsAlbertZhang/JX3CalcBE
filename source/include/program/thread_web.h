#ifndef PROGRAM_THREAD_WEB_H_
#define PROGRAM_THREAD_WEB_H_

#include "program/thread_pool.h"
#pragma warning(push, 0)
#include <crow.h>
#pragma warning(pop)
#include <string>
#include <thread>

namespace ns_program {

class Web {
public:
    Web(); // 构造 WebHandler 的同时, 会启动一个线程
    virtual ~Web();
    Web(const Web &)            = delete;
    Web &operator=(const Web &) = delete;
    Web(Web &&)                 = delete;
    Web &operator=(Web &&)      = delete;

    void stop();

private:
    std::thread webThread;
    void        webEntry();

    crow::SimpleApp app;
    Pool            pool;

    bool task(const std::string &jsonstr);
};

} // namespace ns_program

#endif // PROGRAM_THREAD_WEB_H_
