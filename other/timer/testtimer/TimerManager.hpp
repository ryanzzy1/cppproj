// TimerManager.hpp
#pragma once

#include "Timer.hpp"
#include <queue>
#include <set>
#include <optional>

class TimerManager {
public:
    // 单次定时器
    struct OneShotTask {
        Timer::TimerId id;
        Timer::TimePoint expiry;
        Timer::TimerCallback callback;
        int priority = 0;  // 优先级，数字越小优先级越高
        
        bool operator<(const OneShotTask& other) const {
            if (expiry != other.expiry) {
                return expiry > other.expiry;  // 最小堆，最早到期优先
            }
            return priority < other.priority;  // 相同时间，优先级高优先
        }
    };

    // 周期定时器
    struct PeriodicTask {
        Timer::TimerId id;
        Timer::Duration interval;
        Timer::TimePoint nextExpiry;
        Timer::TimerCallback callback;
        bool isActive = true;
    };

    TimerManager() = default;
    ~TimerManager() {
        stop();
    }

    // 启动管理器
    void start() {
        if (running_) return;
        
        running_ = true;
        managerThread_ = std::thread([this]() { managerThreadFunc(); });
    }

    // 停止管理器
    void stop() {
        running_ = false;
        cv_.notify_all();
        
        if (managerThread_.joinable()) {
            managerThread_.join();
        }
        
        timer_.cancelAll();
        std::unique_lock<std::mutex> lock(mutex_);
        oneShotTasks_ = std::priority_queue<OneShotTask>();
        periodicTasks_.clear();
    }

    // 添加单次定时任务
    Timer::TimerId scheduleOneShot(Timer::Duration delay, 
                                  Timer::TimerCallback callback,
                                  int priority = 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        Timer::TimerId id = timer_.asyncAfter(delay, [this, callback, id]() {
            callback();
            removeOneShotTask(id);
        });
        
        OneShotTask task{
            .id = id,
            .expiry = Timer::Clock::now() + delay,
            .callback = std::move(callback),
            .priority = priority
        };
        
        oneShotTasks_.push(task);
        cv_.notify_one();
        return id;
    }

    // 添加周期定时任务
    Timer::TimerId schedulePeriodic(Timer::Duration interval,
                                   Timer::TimerCallback callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        Timer::TimerId id = nextId_++;
        
        PeriodicTask task{
            .id = id,
            .interval = interval,
            .nextExpiry = Timer::Clock::now() + interval,
            .callback = std::move(callback)
        };
        
        periodicTasks_[id] = task;
        
        // 启动第一个周期
        scheduleNextPeriodic(id);
        cv_.notify_one();
        return id;
    }

    // 取消任务
    bool cancel(Timer::TimerId id) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // 检查是否为单次任务
        std::priority_queue<OneShotTask> newQueue;
        bool found = false;
        
        while (!oneShotTasks_.empty()) {
            auto task = oneShotTasks_.top();
            oneShotTasks_.pop();
            
            if (task.id == id) {
                found = true;
                timer_.cancel(id);
            } else {
                newQueue.push(task);
            }
        }
        
        oneShotTasks_ = std::move(newQueue);
        
        // 检查是否为周期任务
        auto it = periodicTasks_.find(id);
        if (it != periodicTasks_.end()) {
            found = true;
            timer_.cancel(id);
            periodicTasks_.erase(it);
        }
        
        return found;
    }

    // 获取下一个任务的剩余时间
    std::optional<Timer::Duration> nextTaskDelay() const {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (oneShotTasks_.empty() && periodicTasks_.empty()) {
            return std::nullopt;
        }
        
        Timer::TimePoint nextExpiry = Timer::TimePoint::max();
        
        if (!oneShotTasks_.empty()) {
            nextExpiry = std::min(nextExpiry, oneShotTasks_.top().expiry);
        }
        
        for (const auto& pair : periodicTasks_) {
            nextExpiry = std::min(nextExpiry, pair.second.nextExpiry);
        }
        
        return std::max(Timer::Duration{0}, nextExpiry - Timer::Clock::now());
    }

private:
    Timer timer_;
    std::atomic<bool> running_{false};
    std::thread managerThread_;
    
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::priority_queue<OneShotTask> oneShotTasks_;
    std::unordered_map<Timer::TimerId, PeriodicTask> periodicTasks_;
    std::atomic<Timer::TimerId> nextId_{1};

    // 移除单次任务
    void removeOneShotTask(Timer::TimerId id) {
        std::unique_lock<std::mutex> lock(mutex_);
        std::priority_queue<OneShotTask> newQueue;
        
        while (!oneShotTasks_.empty()) {
            auto task = oneShotTasks_.top();
            oneShotTasks_.pop();
            
            if (task.id != id) {
                newQueue.push(task);
            }
        }
        
        oneShotTasks_ = std::move(newQueue);
    }

    // 调度下一个周期任务
    void scheduleNextPeriodic(Timer::TimerId id) {
        auto it = periodicTasks_.find(id);
        if (it == periodicTasks_.end()) return;
        
        auto& task = it->second;
        if (!task.isActive) return;
        
        timer_.asyncAfter(task.interval, [this, id]() {
            std::unique_lock<std::mutex> lock(mutex_);
            auto it = periodicTasks_.find(id);
            if (it == periodicTasks_.end()) return;
            
            auto& task = it->second;
            task.callback();
            
            // 重新调度
            if (task.isActive) {
                task.nextExpiry = Timer::Clock::now() + task.interval;
                scheduleNextPeriodic(id);
            }
        });
    }

    // 管理器线程函数
    void managerThreadFunc() {
        while (running_) {
            std::unique_lock<std::mutex> lock(mutex_);
            
            if (oneShotTasks_.empty() && periodicTasks_.empty()) {
                cv_.wait(lock);
                continue;
            }
            
            auto delay = nextTaskDelay();
            if (!delay.has_value()) {
                cv_.wait(lock);
                continue;
            }
            
            cv_.wait_for(lock, delay.value());
        }
    }
};