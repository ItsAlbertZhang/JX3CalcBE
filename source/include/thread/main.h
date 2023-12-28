#ifndef THREAD_MAIN_H_
#define THREAD_MAIN_H_

#include "thread/child.h"
#include "thread/pool.h"

namespace ns_thread {

class Main {
public:
    Main();
    ~Main();
    Main(const Main &)            = delete;
    Main &operator=(const Main &) = delete;
    Main(Main &&)                 = delete;
    Main &operator=(Main &&)      = delete;

    void run();

    static void cleanup();

private:
    Child child;
    Pool  pool;
};

} // namespace ns_thread

#endif // THREAD_MAIN_H_
