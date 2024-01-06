// https://stackoverflow.com/a/4793662/3583290
// by Maxim Egorushkin

#ifndef INCLUDED_AQ_Event_H
#define INCLUDED_AQ_Event_H

#include <mutex>
#include <condition_variable>

template<bool auto_reset = true>
class Event
{
private:
    std::mutex mutex_;
    std::condition_variable condition_;
    bool set_ = false; // Initialized as locked.

public:
    void set() {
        std::lock_guard<decltype(mutex_)> lock(mutex_);
        set_ = true;
        if (auto_reset)
            condition_.notify_one();
        else
            condition_.notify_all();
    }
    void reset() {
        std::lock_guard<decltype(mutex_)> lock(mutex_);
        set_ = false;
    }
    void wait() {
        std::unique_lock<decltype(mutex_)> lock(mutex_);
        while (!set_) // Handle spurious wake-ups.
            condition_.wait(lock);
        if (auto_reset)
            set_ = false;
    }
};

#endif
