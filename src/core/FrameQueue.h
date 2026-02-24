#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

#include "Frame.h"

class FrameQueue
{
public:
    void push(const Frame &f)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(f);
        }
        cv_.notify_one();
    }

    bool pop(Frame &out)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        cv_.wait(lock, [&]
                 { return !queue_.empty() || !running_; });

        if (queue_.empty())
            return false;

        out = queue_.front();
        queue_.pop();
        return true;
    }
    bool tryPop(Frame &out)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (queue_.empty())
            return false;

        out = queue_.front();
        queue_.pop();
        return true;
    }

    void stop()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
        cv_.notify_all();
    }

private:
    std::queue<Frame> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool running_ = true;
};