#pragma once

#include "Frame.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>

class FrameQueue
{
public:
    void push(const Frame& f)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(f);
        }
        cv_.notify_one();
    }

    // Blokujące oczekiwanie z timeoutem
    bool waitAndPop(Frame& out,
                    std::chrono::milliseconds timeout)
    {
        std::unique_lock<std::mutex> lock(mutex_);

        cv_.wait_for(lock, timeout, [&]{
            return !queue_.empty() || !running_;
        });

        if(!running_)
            return false;

        if(queue_.empty())
            return true;   // timeout (tick)

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

    bool isRunning() const
    {
        return running_;
    }

private:
    std::queue<Frame> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool running_ = true;
};