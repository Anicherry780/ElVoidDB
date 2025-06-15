#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <condition_variable>
#include <atomic>

namespace elvoiddb::util {

class ThreadPool {
    std::vector<std::thread>              workers_;
    std::queue<std::function<void()>>     tasks_;
    std::mutex                            mtx_;
    std::condition_variable               cv_;
    std::atomic<bool>                     shutdown_{false};

    void worker() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock lock(mtx_);
                cv_.wait(lock, [&]{ return shutdown_ || !tasks_.empty(); });
                if (shutdown_ && tasks_.empty()) return;
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            task();
        }
    }
public:
    explicit ThreadPool(size_t n = std::thread::hardware_concurrency()) {
        for (size_t i = 0; i < n; ++i) workers_.emplace_back([this]{ worker(); });
    }
    ~ThreadPool() {
        {
            std::lock_guard lock(mtx_);
            shutdown_ = true;
        }
        cv_.notify_all();
        for (auto& w : workers_) w.join();
    }
    template <typename F>
    void submit(F&& f) {
        {
            std::lock_guard lock(mtx_);
            tasks_.emplace(std::forward<F>(f));
        }
        cv_.notify_one();
    }
};

/* global worker pool (created once, used everywhere) */
extern ThreadPool gThreadPool;

} // namespace elvoiddb::util
