// Timer.hpp
#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

class Timer {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = Clock::duration;
    using TimerCallback = std::function<void()>;

    // 定时器ID类型
    using TimerId = uint64_t;

    // 定时器状态
    enum class State {
        IDLE,       // 空闲
        SCHEDULED,  // 已调度
        RUNNING,    // 执行中
        CANCELLED,  // 已取消
        EXPIRED     // 已过期
    };

    // 构造函数
    Timer() = default;
    
    // 析构函数 - 自动取消所有定时器
    ~Timer() {
        cancelAll();
    }

    // 禁止拷贝
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    // 阻塞定时 - 返回是否成功（false表示被取消）
    template<typename Rep, typename Period>
    bool blockFor(const std::chrono::duration<Rep, Period>& duration) {
        TimerId id = generateId();
        
        {
            std::unique_lock<std::mutex> lock(mutex_);
            auto& timerInfo = timers_[id];
            timerInfo.state = State::SCHEDULED;
            timerInfo.expiry = Clock::now() + duration;
            timerInfo.cv = std::make_unique<std::condition_variable>();
        }

        std::unique_lock<std::mutex> lock(mutex_);
        auto& timerInfo = timers_[id];
        
        // 等待定时到期或被取消
        while (timerInfo.state == State::SCHEDULED) {
            if (timerInfo.cv->wait_for(lock, duration) == std::cv_status::timeout) {
                if (timerInfo.state == State::SCHEDULED) {
                    timerInfo.state = State::EXPIRED;
                    timers_.erase(id);
                    return true;  // 定时成功
                }
            }
        }
        
        // 被取消
        timers_.erase(id);
        return false;
    }

    // 异步定时 - 返回future，可以获取结果或取消
    template<typename Rep, typename Period>
    std::future<bool> asyncAfter(const std::chrono::duration<Rep, Period>& duration,
                                TimerCallback callback = nullptr) {
        TimerId id = generateId();
        auto promise = std::make_shared<std::promise<bool>>();
        std::future<bool> future = promise->get_future();

        {
            std::unique_lock<std::mutex> lock(mutex_);
            auto& timerInfo = timers_[id];
            timerInfo.state = State::SCHEDULED;
            timerInfo.expiry = Clock::now() + duration;
            timerInfo.callback = std::move(callback);
            timerInfo.promise = promise;
        }

        // 启动异步线程
        std::thread([this, id, duration, promise]() {
            asyncTimerThread(id, duration, promise);
        }).detach();

        return future;
    }

    // 异步定时循环
    template<typename Rep, typename Period>
    TimerId asyncEvery(const std::chrono::duration<Rep, Period>& interval,
                      TimerCallback callback) {
        TimerId id = generateId();
        
        {
            std::unique_lock<std::mutex> lock(mutex_);
            auto& timerInfo = timers_[id];
            timerInfo.state = State::SCHEDULED;
            timerInfo.expiry = Clock::now() + interval;
            timerInfo.callback = callback;
            timerInfo.interval = interval;
            timerInfo.isPeriodic = true;
        }

        // 启动周期定时线程
        std::thread([this, id, interval, callback]() {
            periodicTimerThread(id, interval, callback);
        }).detach();

        return id;
    }

    // 取消指定定时器
    bool cancel(TimerId id) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto it = timers_.find(id);
        
        if (it == timers_.end()) {
            return false;  // 定时器不存在
        }

        auto& timerInfo = it->second;
        
        if (timerInfo.state == State::SCHEDULED || timerInfo.state == State::RUNNING) {
            timerInfo.state = State::CANCELLED;
            
            // 通知条件变量（如果是阻塞定时）
            if (timerInfo.cv) {
                timerInfo.cv->notify_all();
            }
            
            // 设置promise结果（如果是异步定时）
            if (timerInfo.promise) {
                timerInfo.promise->set_value(false);
            }
            
            timers_.erase(id);
            return true;
        }
        
        return false;
    }

    // 取消所有定时器
    void cancelAll() {
        std::unique_lock<std::mutex> lock(mutex_);
        
        for (auto& pair : timers_) {
            auto& timerInfo = pair.second;
            
            if (timerInfo.state == State::SCHEDULED || timerInfo.state == State::RUNNING) {
                timerInfo.state = State::CANCELLED;
                
                if (timerInfo.cv) {
                    timerInfo.cv->notify_all();
                }
                
                if (timerInfo.promise) {
                    try {
                        timerInfo.promise->set_value(false);
                    } catch (const std::future_error&) {
                        // 忽略promise已设置的情况
                    }
                }
            }
        }
        
        timers_.clear();
    }

    // 获取定时器状态
    State getState(TimerId id) const {
        std::unique_lock<std::mutex> lock(mutex_);
        auto it = timers_.find(id);
        return it != timers_.end() ? it->second.state : State::IDLE;
    }

    // 获取活跃定时器数量
    size_t activeCount() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return timers_.size();
    }

private:
    struct TimerInfo {
        State state = State::IDLE;
        TimePoint expiry;
        Duration interval{0};
        TimerCallback callback;
        std::unique_ptr<std::condition_variable> cv;
        std::shared_ptr<std::promise<bool>> promise;
        bool isPeriodic = false;
    };

    mutable std::mutex mutex_;
    std::unordered_map<TimerId, TimerInfo> timers_;
    std::atomic<TimerId> nextId_{1};

    // 生成唯一ID
    TimerId generateId() {
        return nextId_++;
    }

    // 异步定时器线程函数
    template<typename Rep, typename Period>
    void asyncTimerThread(TimerId id, 
                         const std::chrono::duration<Rep, Period>& duration,
                         std::shared_ptr<std::promise<bool>> promise) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto it = timers_.find(id);
        
        if (it == timers_.end()) {
            promise->set_value(false);
            return;
        }

        auto& timerInfo = it->second;
        timerInfo.state = State::RUNNING;
        
        // 等待定时到期或被取消
        auto cv = std::make_unique<std::condition_variable>();
        while (timerInfo.state == State::RUNNING) {
            if (cv->wait_for(lock, duration) == std::cv_status::timeout) {
                if (timerInfo.state == State::RUNNING) {
                    timerInfo.state = State::EXPIRED;
                    
                    // 执行回调函数
                    if (timerInfo.callback) {
                        lock.unlock();
                        timerInfo.callback();
                        lock.lock();
                    }
                    
                    promise->set_value(true);
                    timers_.erase(id);
                    return;
                }
            }
        }
        
        // 被取消
        promise->set_value(false);
        timers_.erase(id);
    }

    // 周期定时器线程函数
    template<typename Rep, typename Period>
    void periodicTimerThread(TimerId id,
                           const std::chrono::duration<Rep, Period>& interval,
                           TimerCallback callback) {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex_);
            auto it = timers_.find(id);
            
            if (it == timers_.end() || it->second.state != State::SCHEDULED) {
                break;
            }

            auto& timerInfo = it->second;
            timerInfo.state = State::RUNNING;
            
            // 计算下一个到期时间
            auto nextExpiry = Clock::now() + interval;
            
            // 使用条件变量等待
            auto cv = std::make_unique<std::condition_variable>();
            while (timerInfo.state == State::RUNNING && Clock::now() < nextExpiry) {
                auto remaining = nextExpiry - Clock::now();
                if (cv->wait_for(lock, remaining) == std::cv_status::timeout) {
                    break;
                }
            }
            
            if (timerInfo.state == State::CANCELLED) {
                timers_.erase(id);
                break;
            }
            
            if (Clock::now() >= nextExpiry) {
                // 执行回调
                lock.unlock();
                callback();
                lock.lock();
                
                // 重新调度
                if (timerInfo.state == State::RUNNING) {
                    timerInfo.state = State::SCHEDULED;
                    timerInfo.expiry = Clock::now() + interval;
                }
            }
        }
    }
};