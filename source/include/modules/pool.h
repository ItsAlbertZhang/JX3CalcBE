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

namespace jx3calc {
namespace modules {

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
        bool empty();

        // 从当前结点 (任务列) 中取出一个任务, 并将当前结点指针指向下一个结点 (任务列).
        // 注意! 使用前应当先调用 empty() 方法检查 TaskList 是否为空.
        std::function<void()> pop();

        // 向指定 ID 的任务列中添加一个任务 (若 ID 不存在则创建)
        // 注意! 需要自行保证 ID 的唯一性.
        void emplace(const std::string &id, std::function<void()> task);

        // 删除指定 ID 的结点 (任务列)
        void erase(const std::string &id);

        void clear();
    };

public:
    Pool();

    template <class F, class... Args>
    void emplace(const std::string &id, int count, std::vector<std::future<typename std::invoke_result<F, Args...>::type>> &res, F &&f, Args &&...args);

    void erase(const std::string &id);

    void join();

private:
    std::vector<std::thread> workers;
    TaskList                 tasks;
    std::mutex               queue_mutex;
    std::condition_variable  condition;
    bool                     stop = false;
};

} // namespace modules
} // namespace jx3calc

#endif // MODULES_POOL_H_
