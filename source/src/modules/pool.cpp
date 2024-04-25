#include "modules/pool.h"
#include "modules/task.h"

using namespace jx3calc;
using namespace modules;

bool Pool::TaskList::empty() {
    return curr == nullptr; // 当 curr 为 nullptr 时, 所有任务均已完成.
    // 只要存在任意一个结点, 那么 curr 就不会为 nullptr, curr->tasks 也一定还有任务没有完成.
}

std::function<void()> Pool::TaskList::pop() {
    std::function<void()> task = std::move(curr->tasks.front()); // 从 curr->tasks 取出一个任务
    curr->tasks.pop();                                           // 从 curr->tasks 中删除这个任务

    if (curr->tasks.empty()) { // 若当前结点的任务队列为空
        // 则将 curr 指向下一个结点, 并删除原先的结点
        Node *node = curr;
        if (curr->next == curr) // 若当前结点已是 TaskList 最后一个结点:
            curr = nullptr;     // 将 curr 置空, 此时 TaskList 为空.
        else                    // 否则 (当前结点不是最后一个结点):
            curr = curr->next;  // 将 curr 指向下一个结点.
        // 删除四步骤: 前, 后, 哈希表, 删除.
        node->next->prev = node->prev;
        node->prev->next = node->next;
        map.erase(node->id);
        delete node;
    } else {               // 若当前结点的任务队列不为空
        curr = curr->next; // 则直接将 curr 指向下一个结点
    }
    return task;
}

void Pool::TaskList::emplace(const std::string &id, std::function<void()> task) {
    if (!map.contains(id)) {
        // 如果哈希表中不存在该 ID, 则新建一个结点 (任务列)
        Node *node = new Node{.id = id};
        if (curr == nullptr) {
            // 若当前 TaskList 为空, 则将新建的结点作为唯一结点.
            node->next = node;
            node->prev = node;
            curr       = node;
        } else {
            // 若当前 TaskList 不为空, 则将新建的结点插入到当前结点的前面.
            node->next       = curr;
            node->prev       = curr->prev;
            node->next->prev = node;
            node->prev->next = node;
            curr             = node;
        }
        // 将新建的结点插入到哈希表中.
        map.emplace(id, node);
    }
    // 将任务添加到指定批次的任务队列中.
    map.at(id)->tasks.emplace(std::move(task));
}

void Pool::TaskList::erase(const std::string &id) {
    if (!map.contains(id))
        return;
    Node *node = map.at(id);
    if (node == curr) {
        // 如果要删除的是当前结点
        if (curr->next == curr) // 若当前结点已是 TaskList 最后一个结点:
            curr = nullptr;     // 将 curr 置空, 此时 TaskList 为空.
        else                    // 否则 (当前结点不是最后一个结点):
            curr = curr->next;  // 将 curr 指向下一个结点.
    }
    // 删除四步骤: 前, 后, 哈希表, 删除.
    node->next->prev = node->prev;
    node->prev->next = node->next;
    map.erase(id);
    delete node;
}

void Pool::TaskList::clear() {
    while (curr != nullptr) {
        Node *node = curr;
        if (curr->next == curr) // 若当前结点已是 TaskList 最后一个结点:
            curr = nullptr;     // 将 curr 置空, 此时 TaskList 为空.
        else                    // 否则 (当前结点不是最后一个结点):
            curr = curr->next;  // 将 curr 指向下一个结点.
        map.erase(node->id);
        delete node;
    }
}

Pool::Pool() {
#if defined(D_CONSTEXPR_LOG) || defined(D_CONSTEXPR_CHANNELINTERVAL)
    constexpr unsigned int corecnt = 1;
#else
    const unsigned int corecnt = std::thread::hardware_concurrency();
#endif
    for (unsigned int i = 0; i < corecnt; i++)
        workers.emplace_back([this] {
            // thread_init();
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex); // 上锁
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    if (this->stop) [[unlikely]] {
                        return;
                    }
                    task = this->tasks.pop();
                }
                task();
            }
        });
}

void Pool::erase(const std::string &id) {
    std::unique_lock<std::mutex> lock(queue_mutex); // 上锁
    tasks.erase(id);
}

void Pool::join() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.clear(); // 清空任务队列
        stop = true;   // 向线程标识退出
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
}

template <class F, class... Args>
void Pool::emplace(const std::string &id, int count, std::vector<std::future<typename std::invoke_result<F, Args...>::type>> &res, F &&f, Args &&...args) {
    using return_type = typename std::invoke_result<F, Args...>::type;
    res.reserve(res.size() + count);
    {
        std::unique_lock<std::mutex> lock(queue_mutex); // 上锁
        for (int i = 0; i < count; i++) {
            auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            res.emplace_back(task->get_future());
            tasks.emplace(id, [task]() { (*task)(); }); // 注意此处是传递 task 的拷贝, 而不是引用.
            /** 笔记:
             *  f 的函数指针, 参数和返回值被 packaged_task 打包至 task 中, 以 shared_ptr 的形式储存.
             *  随后, 这个 shared_ptr 在 lambda 函数中被非引用捕获, 存储在一个 std::function<void()> 中. 因此, 其实质上存储在 tasks 中.
             *  重点在于, 经 packaged_task 打包后, 函数的返回值不是 packaged_task 的成员, 而是一个 shared_ptr.
             *  当执行 task->get_future() 时, 返回的 future 也会持有一个引用计数.
             *  当销毁 tasks 中的任务列时, packaged_task 也会随之销毁, 进而作为 shared_ptr 的 task 也被销毁.
             *  但 task 销毁后, 并不会影响在 res 中的 future, 因此它是线程安全的.
             */
        }
    }
    condition.notify_all();
    return;
}

// int calcDetail(const Data &data, frame::ChDamage *detail)
using t1Res  = int;
using t1Arg1 = const task::Task::Data &;
using t1Arg2 = std::vector<frame::Damage> *;
using t1Func = t1Res (&)(t1Arg1, t1Arg2);

template void Pool::emplace<t1Func, t1Arg1, t1Arg2>(
    const std::string &,
    int,
    std::vector<std::future<t1Res>> &,
    t1Func,
    t1Arg1 &&,
    t1Arg2 &&
);

// int calcBrief(const Data &arg)
using t2Res  = int;
using t2Arg1 = const task::Task::Data &;
using t2Func = t2Res (&)(t2Arg1);

template void Pool::emplace<t2Func, t2Arg1>(
    const std::string &,
    int,
    std::vector<std::future<t2Res>> &,
    t2Func,
    t2Arg1 &&
);
