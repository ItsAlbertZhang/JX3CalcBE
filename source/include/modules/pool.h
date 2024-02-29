#ifndef MODULES_POOL_H_
#define MODULES_POOL_H_

#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace ns_modules {

#if defined(D_CONSTEXPR_LOG) || defined(D_CONSTEXPR_CHANNELINTERVAL)
inline constexpr unsigned int corecnt = 1;
#else
inline const unsigned int corecnt = std::thread::hardware_concurrency();
#endif

class Pool {

    /**
     *  TaskList 是一个双向循环链表, 它的每一个结点都是一列任务.
     *  每从当前列任务取出一个任务时, 它就会指向下一列任务, 以保证均匀地响应每一列任务.
     *  注意! 线程安全需自行确保!
     */
    class TaskList {

        /**
         *  Node 是 TaskList 的结点, 它的每一个结点都是一列任务.
         */
        class Node {
        public:
            // 任务队列. 注意, 这个任务队列永不为空, 当任务队列为空时, 就应当进行结点删除操作.
            std::queue<std::function<void()>> tasks;
            std::string                       id;   // 本列任务的 ID
            Node                             *next; // 下一个结点 (下一列任务)
            Node                             *prev; // 上一个结点 (上一列任务)
        };

    private:
        // TaskList 有两种查找结点 (任务列) 的方式:

        // 1. 当前指向的结点 (任务列). 它被 pop 方法使用: 执行 pop 操作会从该结点 (任务列) 中取出一个任务, 并将其指向下一个结点 (任务列).
        Node *curr = nullptr;

        // 2. 哈希表, 通过任务 ID 直接定位任务. 它被 erase 方法使用: 执行 erase 操作会删除指定 ID 的结点 (任务列).
        std::unordered_map<std::string, Node *> map;

    public:
        bool empty() {
            return curr == nullptr; // 当 curr 为 nullptr 时, 所有任务均已完成.
            // 只要存在任意一个结点, 那么 curr 就不会为 nullptr, curr->tasks 也一定还有任务没有完成.
        }

        // 从当前结点 (任务列) 中取出一个任务, 并将当前结点指针指向下一个结点 (任务列).
        // 注意! 使用前应当先调用 empty() 方法检查 TaskList 是否为空.
        std::function<void()> pop() {
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

        // 向指定 ID 的任务列中添加一个任务 (若 ID 不存在则创建)
        // 注意! 需要自行保证 ID 的唯一性.
        void emplace(const std::string &id, std::function<void()> task) {
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

        // 删除指定 ID 的结点 (任务列)
        void erase(const std::string &id) {
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

        void clear() {
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
    };

public:
    Pool(
        // std::function<void()> thread_init    = []() {},
        // std::function<void()> thread_cleanup = []() {}
    )
        : stop(false) {
        for (unsigned int i = 0; i < corecnt; ++i)
            // workers.emplace_back([this, thread_init, thread_cleanup] {
            workers.emplace_back([this] {
                // thread_init();
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex); // 上锁
                        this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                        if (this->stop) [[unlikely]] {
                            // thread_cleanup();
                            return;
                        }
                        task = this->tasks.pop();
                    }
                    task();
                }
            });
    }

    template <class F, class... Args>
    void emplace(const std::string &id, int count, std::vector<std::future<typename std::invoke_result<F, Args...>::type>> &res, F &&f, Args &&...args) {
        using return_type = typename std::invoke_result<F, Args...>::type;
        res.reserve(res.size() + count);
        {
            std::unique_lock<std::mutex> lock(queue_mutex); // 上锁
            for (int i = 0; i < count; ++i) {
                auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
                res.emplace_back(task->get_future());
                tasks.emplace(id, [task]() { (*task)(); });
                /** 笔记:
                 *  f 的函数指针, 参数和返回值被 packaged_task 打包至 task 中, 以 shared_ptr 的形式储存.
                 *  随后, 这个 shared_ptr 在 lambda 函数中被使用, 并存储为 std::function<void()>. 因此, 其实质上存储在 tasks 中.
                 *  重点在于, 其中, f 的返回值实际上并不是 packaged_task 的成员, 而是一个 shared_ptr.
                 *  当执行 task->get_future() 时, 返回的 future 也会持有一个引用计数.
                 *  当销毁 tasks 中的任务列时, packaged_task 也会随之销毁, 进而作为 shared_ptr 的 task 也被销毁.
                 *  但 task 销毁后, 并不会影响在 res 中的 future, 它是线程安全的.
                 */
            }
        }
        condition.notify_all();
        return;
    }

    void erase(const std::string &id) {
        std::unique_lock<std::mutex> lock(queue_mutex); // 上锁
        tasks.erase(id);
    }

    void join() {
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

private:
    std::vector<std::thread> workers;
    TaskList                 tasks;
    std::mutex               queue_mutex;
    std::condition_variable  condition;
    bool                     stop;
};

} // namespace ns_modules

#endif // MODULES_POOL_H_
