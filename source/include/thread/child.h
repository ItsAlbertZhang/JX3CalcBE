#ifndef THREAD_CHILD_H_
#define THREAD_CHILD_H_

#include "thread/main.h"

namespace ns_thread {

class Child {
public:
    Child()                         = default;
    ~Child()                        = default;
    Child(const Child &)            = delete;
    Child &operator=(const Child &) = delete;
    Child(Child &&)                 = delete;
    Child &operator=(Child &&)      = delete;

    void run();

private:
    friend class Main;
};

} // namespace ns_thread

#endif // THREAD_CHILD_H_
