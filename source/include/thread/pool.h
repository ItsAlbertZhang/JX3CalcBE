#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <cassert>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace ns_thread {

class TaskList {
    // 链表结点. 每一个结点对应一批任务 (一个任务队列).
    class Node {
    public:
        std::queue<std::function<void()>> tasks; // 任务队列. 注意, 这个任务队列永不为空.
        std::string                       id;    // 本批任务的哈希 ID
        Node                             *next;  // 下一个结点
        Node                             *prev;  // 上一个结点
    };

private:
    Node                                   *curr = nullptr; // 当前指向的结点, 下次执行任务时从该节点(该批任务)取出一个任务
    std::unordered_map<std::string, Node *> map;            // 哈希表, 每一个元素为一批任务.

public:
    bool empty() {
        return curr == nullptr; // 当 curr 为 nullptr 时, 所有任务均已完成.
        // 只要存在任意一个结点, 那么 curr 就不会为 nullptr, curr->tasks 也一定还有任务没有完成.
    }

    /**
     * @brief 从当前批次的任务队列中取出一个任务, 并自动将当前批次 + 1.
     * @return std::function<void()>
     */
    std::function<void()> pop() {
        assert(!empty()); // 在使用 pop 之前, 应该先使用 empty 判断是否还有任务.
        // 处理任务. 在非空的情况下, curr->tasks 一定有任务没有完成.
        auto task = std::move(curr->tasks.front()); // 从 curr->tasks 取出一个任务
        curr->tasks.pop();                          // 从 curr->tasks 中删除这个任务
        // 任务处理完毕, 处理结点.
        if (curr->tasks.empty()) {
            // 若当前结点的任务队列为空, 则删除该结点. (保证每一个结点的任务队列永不为空)
            Node *node = curr;
            if (curr->next == curr)
                curr = nullptr; // 当前结点已经是最后一个结点. 将 curr 置空, 此时链表为空.
            else
                curr = curr->next; // 当前结点不是最后一个结点. 将 curr 指向下一个结点.
            // 删除四步骤: 哈希表, 前, 后, 删除.
            map.erase(node->id);
            node->next->prev = node->prev;
            node->prev->next = node->next;
            delete node;
        } else
            // 若当前结点的任务队列不为空, 则转而去指向下一个任务队列.
            // 当前结点的任务队列为空时, 已在上文的处理中转而去指向了下一个任务队列.
            curr = curr->next;
        // 也就是说, pop() 方法的逻辑是, 处理一个本批任务, 然后转而去指向下一批任务, 以供下一次 pop() 时处理.
        // 这样, 可以保证均匀地响应每一批任务.
        return task;
    }

    /**
     * @brief 向指定哈希 ID 的任务批次中添加一个任务 (若哈希 ID 对应的任务批次不存在则创建)
     * @param id 任务哈希 ID
     * @param task 任务
     */
    void emplace(std::string id, std::function<void()> task) {
        if (!map.contains(id)) {
            // 如果哈希表中不存在该批任务, 则新建一个结点.
            Node *node = new Node();
            node->id   = id;
            if (curr == nullptr) {
                // 若链表为空, 则将新建的结点作为链表的唯一结点.
                curr       = node;
                node->next = node;
                node->prev = node;
            } else {
                // 若链表不为空, 则将新建的结点插入到当前结点的前面.
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
        map.at(id)->tasks.emplace(task);
    }

    /**
     * @brief 删除指定哈希 ID 的任务批次
     * @param id 任务哈希 ID
     */
    void erase(std::string id) {
        if (!map.contains(id))
            return;
        Node *node = map.at(id);
        if (node == curr) {
            // 如果要删除的是当前结点
            if (curr->next == curr)
                curr = nullptr; // 当前结点已经是最后一个结点. 将 curr 置空, 此时链表为空.
            else
                curr = curr->next; // 当前结点不是最后一个结点. 将 curr 指向下一个结点.
        }
        // 删除四步骤: 哈希表, 前, 后, 删除.
        map.erase(id);
        node->next->prev = node->prev;
        node->prev->next = node->next;
        delete node;
    }
};

class Pool {
public:
    Pool(
        std::function<void()> thread_init    = []() {},
        std::function<void()> thread_cleanup = []() {}
    )
        : stop(false) {
        unsigned int corecnt = std::thread::hardware_concurrency();
        for (unsigned int i = 0; i < corecnt; ++i)
            workers.emplace_back([this, thread_init, thread_cleanup] {
                thread_init();
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty()) {
                            thread_cleanup();
                            return;
                        }
                        task = this->tasks.pop();
                    }
                    task();
                }
            });
    }

    template <class F, class... Args>
    auto enqueue(std::string id, int count, F &&f, Args &&...args) -> std::vector<std::future<typename std::invoke_result<F, Args...>::type>> {
        using return_type = typename std::invoke_result<F, Args...>::type;
        std::vector<std::future<return_type>> res;
        res.reserve(count);
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            for (int i = 0; i < count; ++i) {
                auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
                res.emplace_back(task->get_future());
                if (stop)
                    throw std::runtime_error("enqueue on stopped ThreadPool");
                tasks.emplace(id, [task]() { (*task)(); });
            }
        }
        condition.notify_all();
        return res;
    }

    virtual ~Pool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
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

} // namespace ns_thread

#endif // THREAD_POOL_H_
