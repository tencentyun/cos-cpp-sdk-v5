#pragma once

#include <mutex>
#include <condition_variable>

// C++11信号量实现，用于滑动窗口控制
class Semaphore {
public:
    explicit Semaphore(unsigned int count = 0) : max_count_(count) {}

    bool acquire() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (count_ < max_count_) {
            ++count_;
            return true;
        }
        return false;
    }

    void release() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (count_ <= 0) {
            return;
        }
        --count_;
        condition_.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this]() { return count_ < max_count_; });
    }

    unsigned int get_count() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return count_;
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    unsigned int count_ = 0;
    const unsigned int max_count_;
};