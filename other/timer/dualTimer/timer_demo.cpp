#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <vector>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <map>
#include <csignal>
#include <future>
#include <algorithm>
#include <unordered_set>

// 全局停止标志
std::atomic<bool> should_exit{false};

// 信号处理函数
void signal_handler(int signal) {
    std::cout << "\n[系统] 收到信号 " << signal << "，正在优雅退出..." << std::endl;
    should_exit = true;
}

// 简化版定时器类，具有优雅关闭功能
class GracefulTimer {
public:
    using TimerCallback = std::function<void()>;
    
private:
    struct TimerTask {
        int id;
        std::chrono::steady_clock::time_point expiry_time;
        TimerCallback callback;
        bool is_cancelled = false;
        
        bool operator<(const TimerTask& other) const {
            return expiry_time > other.expiry_time;
        }
    };

    std::atomic<bool> running_{false};
    std::thread manager_thread_;
    std::mutex tasks_mutex_;
    std::mutex callback_mutex_;
    std::condition_variable cv_;
    std::condition_variable all_done_cv_;
    std::priority_queue<TimerTask> tasks_queue_;
    std::map<int, TimerTask> active_tasks_;
    std::atomic<int> next_id_{0};
    std::atomic<int> active_callbacks_{0};  // 活跃回调计数
    std::unordered_set<int> pending_timers_; // 等待执行的回调

public:
    GracefulTimer() = default;
    
    ~GracefulTimer() {
        stop();
    }

    void start() {
        if (running_) return;
        
        running_ = true;
        manager_thread_ = std::thread(&GracefulTimer::managerThread, this);
        std::cout << "[定时器管理器] 已启动" << std::endl;
    }
    
    // 优雅停止：等待所有定时器完成
    void stop(bool wait_for_callbacks = true) {
        if (!running_) return;
        
        std::cout << "[定时器管理器] 正在停止..." << std::endl;
        running_ = false;
        cv_.notify_all();
        
        // 等待管理器线程结束
        if (manager_thread_.joinable()) {
            manager_thread_.join();
        }
        
        // 如果需要，等待所有回调完成
        if (wait_for_callbacks) {
            waitForAllCallbacks();
        }
        
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        tasks_queue_ = std::priority_queue<TimerTask>();
        active_tasks_.clear();
        std::cout << "[定时器管理器] 已完全停止" << std::endl;
    }
    
    // 等待所有回调完成（带超时）
    bool waitForAllCallbacks(std::chrono::milliseconds timeout = std::chrono::seconds(10)) {
        std::cout << "[定时器管理器] 等待回调完成，当前活跃回调: " 
                  << active_callbacks_.load() << std::endl;
        
        auto start = std::chrono::steady_clock::now();
        
        while (active_callbacks_.load() > 0) {
            auto now = std::chrono::steady_clock::now();
            if (now - start >= timeout) {
                std::cout << "[定时器管理器] 等待回调超时，强制退出" << std::endl;
                return false;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // 检查是否应该强制退出
            if (should_exit.load()) {
                std::cout << "[定时器管理器] 收到强制退出信号，中断等待" << std::endl;
                return false;
            }
        }
        
        std::cout << "[定时器管理器] 所有回调已完成" << std::endl;
        return true;
    }
    
    // 阻塞定时器
    bool addBlockingTimer(std::chrono::milliseconds duration, 
                         const std::function<bool()>& should_stop = nullptr) {
        std::cout << "[阻塞定时器] 开始: " << duration.count() << "ms" << std::endl;
        
        auto start = std::chrono::steady_clock::now();
        auto end = start + duration;
        
        while (std::chrono::steady_clock::now() < end) {
            auto remaining = end - std::chrono::steady_clock::now();
            using SteadyDuration = std::chrono::steady_clock::duration;
            auto wait_time = std::min<SteadyDuration>(
                remaining, 
                std::chrono::duration_cast<SteadyDuration>(std::chrono::milliseconds(100)));
            
            std::this_thread::sleep_for(wait_time);
            
            if (should_exit.load()) {
                std::cout << "[阻塞定时器] 收到退出信号，退出" << std::endl;
                return false;
            }
            
            if (should_stop && should_stop()) {
                std::cout << "[阻塞定时器] 外部条件触发，退出" << std::endl;
                return false;
            }
        }
        
        std::cout << "[阻塞定时器] 结束" << std::endl;
        return true;
    }
    
    // 非阻塞定时器
    int addNonBlockingTimer(std::chrono::milliseconds duration, 
                           TimerCallback callback) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        
        if (should_exit.load()) {
            std::cout << "[警告] 程序正在退出，不添加新定时器" << std::endl;
            return -1;
        }
        
        if (!running_) {
            std::cout << "[警告] 定时器管理器已停止，不添加新定时器" << std::endl;
            return -1;
        }
        
        int id = ++next_id_;
        auto now = std::chrono::steady_clock::now();
        auto expiry = now + duration;
        
        // 标记为待处理定时器
        pending_timers_.insert(id);
        
        TimerTask task{
            id,
            expiry,
            [this, id, callback]() {
                // 增加活跃回调计数
                active_callbacks_++;
                std::cout << "[回调开始] ID=" << id 
                          << ", 活跃回调: " << active_callbacks_.load() << std::endl;
                
                try {
                    callback();
                } catch (const std::exception& e) {
                    std::cerr << "[回调异常] ID=" << id << ": " << e.what() << std::endl;
                }
                
                // 减少活跃回调计数
                active_callbacks_--;
                std::cout << "[回调结束] ID=" << id 
                          << ", 活跃回调: " << active_callbacks_.load() << std::endl;
                
                // 从待处理列表中移除
                {
                    std::lock_guard<std::mutex> lock(tasks_mutex_);
                    pending_timers_.erase(id);
                }
                
                // 通知等待线程
                all_done_cv_.notify_one();
            },
            false
        };
        
        tasks_queue_.push(task);
        active_tasks_[id] = task;
        cv_.notify_one();
        
        std::cout << "[非阻塞定时器] 添加: ID=" << id 
                  << ", 时长=" << duration.count() << "ms" 
                  << ", 总活跃回调: " << active_callbacks_.load() << std::endl;
        
        return id;
    }
    
    bool cancelTimer(int timer_id) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        auto it = active_tasks_.find(timer_id);
        if (it != active_tasks_.end()) {
            it->second.is_cancelled = true;
            pending_timers_.erase(timer_id);
            std::cout << "[定时器] 取消: ID=" << timer_id << std::endl;
            return true;
        }
        return false;
    }
    
    // 获取活跃回调数量
    int getActiveCallbackCount() const {
        return active_callbacks_.load();
    }
    
    // 获取待处理定时器数量
    int getPendingTimerCount() const {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        return pending_timers_.size();
    }

private:
    void managerThread() {
        while (running_ || !pending_timers_.empty()) {
            std::unique_lock<std::mutex> lock(tasks_mutex_);
            
            if (tasks_queue_.empty()) {
                // 如果没有待处理的定时器，可以退出
                if (pending_timers_.empty()) {
                    break;
                }
                cv_.wait_for(lock, std::chrono::milliseconds(100));
                continue;
            }
            
            auto next_task = tasks_queue_.top();
            auto now = std::chrono::steady_clock::now();
            
            if (now >= next_task.expiry_time) {
                tasks_queue_.pop();
                
                auto it = active_tasks_.find(next_task.id);
                if (it != active_tasks_.end()) {
                    auto task = it->second;
                    active_tasks_.erase(it);
                    
                    if (!task.is_cancelled) {
                        lock.unlock();  // 释放锁，避免阻塞
                        
                        std::cout << "[管理器] 执行回调: ID=" << task.id << std::endl;
                        
                        // 执行回调
                        try {
                            task.callback();
                        } catch (const std::exception& e) {
                            std::cerr << "[管理器] 回调异常: " << e.what() << std::endl;
                        }
                        
                        lock.lock();  // 重新获取锁
                    } else {
                        pending_timers_.erase(next_task.id);
                    }
                }
            } else {
                auto time_until_wake = next_task.expiry_time - now;
                auto wait_time = std::min<decltype(time_until_wake)>(
                    time_until_wake, 
                    std::chrono::duration_cast<decltype(time_until_wake)>(
                        std::chrono::milliseconds(100)));
                cv_.wait_for(lock, wait_time);
            }
        }
        
        std::cout << "[管理器线程] 退出" << std::endl;
    }
};

// 全局定时器实例
GracefulTimer global_timer;

// 测试用例
int main() {
    std::cout << "=== 优雅定时器示例 ===" << std::endl;
    std::cout << "按 Ctrl+C 可随时终止程序" << std::endl;
    
    // 设置信号处理
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    // 启动定时器管理器
    global_timer.start();
    
    std::vector<int> timer_ids;
    
    // 测试1：快速定时器（应该在主线程退出前完成）
    std::cout << "\n=== 测试1：快速定时器（1秒） ===" << std::endl;
    
    int timer1 = global_timer.addNonBlockingTimer(
        std::chrono::milliseconds(1000),
        []() {
            std::cout << "[快速回调] 1秒后执行" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << "[快速回调] 完成" << std::endl;
        }
    );
    
    if (timer1 > 0) timer_ids.push_back(timer1);
    
    // 测试2：慢速定时器（可能来不及执行）
    std::cout << "\n=== 测试2：慢速定时器（3秒） ===" << std::endl;
    
    int timer2 = global_timer.addNonBlockingTimer(
        std::chrono::milliseconds(3000),
        []() {
            std::cout << "[慢速回调] 3秒后执行" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::cout << "[慢速回调] 完成" << std::endl;
        }
    );
    
    if (timer2 > 0) timer_ids.push_back(timer2);
    
    // 测试3：更慢的定时器（可能被中断）
    std::cout << "\n=== 测试3：更慢的定时器（5秒） ===" << std::endl;
    
    int timer3 = global_timer.addNonBlockingTimer(
        std::chrono::milliseconds(5000),
        []() {
            std::cout << "[更慢回调] 5秒后执行" << std::endl;
            for (int i = 0; i < 10; i++) {
                std::cout << "[更慢回调] 步骤 " << i << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                
                // 检查是否应该退出
                if (should_exit.load()) {
                    std::cout << "[更慢回调] 收到退出信号，中断" << std::endl;
                    return;
                }
            }
            std::cout << "[更慢回调] 完成" << std::endl;
        }
    );
    
    if (timer3 > 0) timer_ids.push_back(timer3);
    
    // 等待一段时间，让部分定时器执行
    std::cout << "\n=== 主线程等待2秒... ===" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "\n=== 当前状态 ===" << std::endl;
    std::cout << "活跃回调: " << global_timer.getActiveCallbackCount() << std::endl;
    std::cout << "待处理定时器: " << global_timer.getPendingTimerCount() << std::endl;
    
    // 模拟收到退出信号
    std::cout << "\n=== 模拟收到退出信号... ===" << std::endl;
    should_exit.store(true);
    
    // 优雅停止定时器（等待所有回调完成）
    std::cout << "\n=== 优雅停止定时器 ===" << std::endl;
    global_timer.stop(true);  // true表示等待所有回调完成
    
    std::cout << "\n=== 程序退出 ===" << std::endl;
    return 0;
}