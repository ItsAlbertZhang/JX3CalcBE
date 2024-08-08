#pragma once

#include <condition_variable>
#include <future>
#include <list>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace jx3calc {
namespace modules {

template <typename return_type>
class Pool {

private:
    std::vector<std::thread> workers;
    std::mutex               mtx;
    std::condition_variable  cond;
    bool                     stop = false;

    struct Task {
        std::queue<std::packaged_task<return_type()>> queue;
        std::string                                   id;
    };
    std::list<Task>           tasks;
    std::list<Task>::iterator curr = tasks.end();

    using TaskIter = typename std::list<Task>::iterator;
    std::unordered_map<std::string, TaskIter> map;

public:
    Pool() {
#if defined(D_CONSTEXPR_LOG) || defined(D_CONSTEXPR_CHANNELINTERVAL)
        constexpr unsigned int corecnt = 1;
#else
        const unsigned int corecnt = std::thread::hardware_concurrency();
#endif
        for (unsigned int i = 0; i < corecnt; i++)
            workers.emplace_back([this] {
                // thread_init();
                while (true) {
                    std::packaged_task<return_type()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->mtx); // 上锁
                        this->cond.wait(lock, [this] { return this->stop || this->curr != tasks.end(); });
                        if (this->stop) [[unlikely]] {
                            return;
                        }
                        // curr != tasks.end()
                        task = std::move(this->curr->queue.front()); // 从 curr 中获取一个任务
                        this->curr->queue.pop();                     // 从 curr 的队列中删除这个任务
                        if (this->curr->queue.empty()) {             // 若 curr 的队列为空
                            this->map.erase(this->curr->id);         // 从哈希表中删除
                            curr = this->tasks.erase(this->curr);    // 从链表中删除, 并将 curr 指向下一个结点
                        } else {                                     // 否则
                            curr++;                                  // 直接将 curr 指向下一个结点
                        }
                        if (curr == tasks.end()) { // 若 curr 已经是最后一个结点
                            curr = tasks.begin();  // 则将 curr 指向第一个结点(可能仍为 tasks.end())
                        }
                    }
                    task();
                }
            });
    }

    template <class F, class... Args>
    void emplace(const std::string &id, int count, std::vector<std::future<return_type>> &res, F &&f, Args &&...args) {
        res.reserve(res.size() + count);
        {
            std::unique_lock<std::mutex>       lock(mtx); // 上锁
            typename std::list<Task>::iterator it;
            if (map.contains(id)) {                        // 若哈希表中已经存在 id
                it = map.at(id);                           // 则直接从哈希表中由 id 获取迭代器
            } else {                                       // 否则
                it = tasks.emplace(curr, Task {.id = id}); // 在 curr 之前插入一个新的 Task
                map.emplace(id, it);                       // 将 id 和迭代器插入哈希表
                if (curr == tasks.end()) {                 // 若 curr 已经是最后一个结点(当前没有任务)
                    curr = it;                             // 则将 curr 指向新插入的结点
                }
            }
            for (int i = 0; i < count; i++) {
                auto task = std::packaged_task<return_type()>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
                res.emplace_back(task.get_future());
                it->queue.emplace(std::move(task));
            }
        }
        cond.notify_all();
        return;
    }

    void erase(const std::string &id) {
        if (!map.contains(id))
            return;
        std::unique_lock<std::mutex>        lock(mtx); // 上锁
        typename std::list<Task>::iterator &it = map.at(id);
        if (it == curr) {
            curr++;
        }
        tasks.erase(it);
        map.erase(id);
    }

    void join() {
        {
            std::unique_lock<std::mutex> lock(mtx);
            tasks.clear(); // 清空任务队列
            map.clear();   // 清空哈希表
            stop = true;   // 向线程标识退出
        }
        cond.notify_all();
        for (std::thread &worker : workers) {
            worker.join();
        }
    }
};

} // namespace modules
} // namespace jx3calc
