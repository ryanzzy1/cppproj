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
    mutable std::mutex tasks_mutex_;  // 添加 mutable
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
        
        // 等待管理器线程真正开始运行
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
        pending_timers_.clear();
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
        return static_cast<int>(pending_timers_.size());
    }
    
    // 检查管理器是否正在运行
    bool isRunning() const {
        return running_.load();
    }

private:
    void managerThread() {
        std::cout << "[管理器线程] 开始运行" << std::endl;
        
        while (running_.load()) {
            std::unique_lock<std::mutex> lock(tasks_mutex_);
            
            // 如果任务队列为空，等待新的任务
            if (tasks_queue_.empty()) {
                cv_.wait_for(lock, std::chrono::milliseconds(100), 
                            [this]() { return !tasks_queue_.empty() || !running_.load(); });
                
                if (!running_.load()) {
                    break;
                }
                
                if (tasks_queue_.empty()) {
                    continue;
                }
            }
            
            auto next_task = tasks_queue_.top();
            auto now = std::chrono::steady_clock::now();
            
            if (now >= next_task.expiry_time) {
                // 定时器到期
                tasks_queue_.pop();
                
                auto it = active_tasks_.find(next_task.id);
                if (it != active_tasks_.end()) {
                    auto task = it->second;
                    active_tasks_.erase(it);
                    
                    if (!task.is_cancelled) {
                        // 释放锁，执行回调
                        lock.unlock();
                        
                        std::cout << "[管理器] 执行回调: ID=" << task.id << std::endl;
                        
                        // 执行回调
                        try {
                            task.callback();
                        } catch (const std::exception& e) {
                            std::cerr << "[管理器] 回调异常: " << e.what() << std::endl;
                        }
                        
                        lock.lock();
                    } else {
                        // 已取消的定时器，从待处理列表中移除
                        pending_timers_.erase(next_task.id);
                        std::cout << "[管理器] 跳过已取消的定时器: ID=" << next_task.id << std::endl;
                    }
                } else {
                    // 定时器已被移除
                    pending_timers_.erase(next_task.id);
                }
            } else {
                // 等待直到下一个定时器到期
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

// 简化的测试用例
int main() {
    std::cout << "=== 阻塞与非阻塞定时器混合示例 ===" << std::endl;
    std::cout << "按 Ctrl+C 可随时终止程序" << std::endl;
    
    // 设置信号处理
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    // 启动定时器管理器
    global_timer.start();
    
    // 检查管理器是否真的启动了
    if (!global_timer.isRunning()) {
        std::cerr << "错误: 定时器管理器启动失败" << std::endl;
        return 1;
    }
    
    std::vector<int> timer_ids;

    // 示例1: 在主线程中直接调用阻塞定时器
    std::cout << "\n=== 示例1: 主线程中的阻塞定时器 ===" << std::endl;
    
    // 主线程会阻塞3秒
    std::cout << "[主线程] 开始执行阻塞定时器 (3秒)" << std::endl;
    bool block_result1 = global_timer.addBlockingTimer(std::chrono::seconds(3), []() {
        return should_exit.load();
    });
    
    std::cout << "[主线程] 阻塞定时器结果: " << (block_result1 ? "成功" : "被中断") << std::endl;
    std::cout << "[主线程] 阻塞定时器执行完毕，主线程继续执行" << std::endl; 
    
    // 示例2: 在独立线程中执行阻塞定时器
    std::cout << "\n=== 示例2: 独立线程中的阻塞定时器 ===" << std::endl;
    
    std::thread blocking_thread1([]() {
        std::cout << "[阻塞线程1] 开始执行阻塞定时器 (2秒)" << std::endl;
        bool block_result = global_timer.addBlockingTimer(std::chrono::seconds(2), []() {
            return should_exit.load();
        });
        std::cout << "[阻塞线程1] 阻塞定时器结果: " << (block_result ? "成功" : "被中断") << std::endl;
    });
    
    // 示例3: 添加非阻塞定时器
    std::cout << "\n=== 示例3: 添加非阻塞定时器 ===" << std::endl;
    
    // 非阻塞定时器1：快速完成
    int timer1 = global_timer.addNonBlockingTimer(
        std::chrono::milliseconds(1500),
        []() {
            std::cout << "[非阻塞定时器1] 1.5秒后执行" << std::endl;
            std::cout << "[非阻塞定时器1] 当前线程ID: " << std::this_thread::get_id() << std::endl;
        }
    );
    
    if (timer1 > 0) timer_ids.push_back(timer1);
    
    // 非阻塞定时器2：需要一些时间
    int timer2 = global_timer.addNonBlockingTimer(
        std::chrono::milliseconds(2500),
        []() {
            std::cout << "[非阻塞定时器2] 2.5秒后执行" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << "[非阻塞定时器2] 完成" << std::endl;
        }
    );
    
    if (timer2 > 0) timer_ids.push_back(timer2);
    
    // 等待阻塞线程1完成
    if (blocking_thread1.joinable()) {
        blocking_thread1.join();
    }
    
    // 示例4: 在主线程中添加另一个阻塞定时器
    std::cout << "\n=== 示例4: 主线程中带中断检查的阻塞定时器 ===" << std::endl;
    
    // 启动一个线程，在1秒后设置should_exit标志
    std::thread interrupt_thread([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "[中断线程] 设置退出标志" << std::endl;
        should_exit.store(true);
    });
    
    std::cout << "[主线程] 开始执行阻塞定时器 (3秒，但会被中断)" << std::endl;
    bool block_result2 = global_timer.addBlockingTimer(std::chrono::seconds(3), []() {
        return should_exit.load();
    });
    
    std::cout << "[主线程] 阻塞定时器结果: " << (block_result2 ? "成功" : "被中断") << std::endl;
    
    // 等待中断线程完成
    if (interrupt_thread.joinable()) {
        interrupt_thread.join();
    }
    
    // 重置退出标志，以便继续测试
    should_exit.store(false);
    
    // 示例5: 添加更多非阻塞定时器
    std::cout << "\n=== 示例5: 添加更多非阻塞定时器 ===" << std::endl;
    
    // 非阻塞定时器3：长时间运行
    int timer3 = global_timer.addNonBlockingTimer(
        std::chrono::milliseconds(2000),
        []() {
            std::cout << "[非阻塞定时器3] 2秒后执行" << std::endl;
            for (int i = 0; i < 5; i++) {
                std::cout << "[非阻塞定时器3] 步骤 " << i << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
            std::cout << "[非阻塞定时器3] 完成" << std::endl;
        }
    );
    
    if (timer3 > 0) timer_ids.push_back(timer3);
    
    // 非阻塞定时器4：在回调中调用阻塞定时器
    int timer4 = global_timer.addNonBlockingTimer(
        std::chrono::milliseconds(1000),
        []() {
            std::cout << "[非阻塞定时器4] 1秒后执行" << std::endl;
            std::cout << "[非阻塞定时器4] 在回调中执行阻塞定时器 (1秒)" << std::endl;
            bool inner_result = global_timer.addBlockingTimer(std::chrono::seconds(1), []() {
                return should_exit.load();
            });
            std::cout << "[非阻塞定时器4] 内部阻塞定时器结果: " << (inner_result ? "成功" : "被中断") << std::endl;
            std::cout << "[非阻塞定时器4] 完成" << std::endl;
        }
    );
    
    if (timer4 > 0) timer_ids.push_back(timer4);
    
    std::cout << "\n=== 当前状态 ===" << std::endl;
    std::cout << "活跃回调: " << global_timer.getActiveCallbackCount() << std::endl;
    std::cout << "待处理定时器: " << global_timer.getPendingTimerCount() << std::endl;
    
    // 等待足够时间让所有定时器执行
    std::cout << "\n=== 等待所有定时器执行 (4秒) ===" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(4));
    
    std::cout << "\n=== 最终状态 ===" << std::endl;
    std::cout << "活跃回调: " << global_timer.getActiveCallbackCount() << std::endl;
    std::cout << "待处理定时器: " << global_timer.getPendingTimerCount() << std::endl;
 
    // 示例6: 演示随时取消定时器
    std::cout << "\n=== 示例6: 取消定时器演示 ===" << std::endl;
    
    // 添加一个5秒的非阻塞定时器
    int timer5 = global_timer.addNonBlockingTimer(
        std::chrono::seconds(5),
        []() {
            std::cout << "[定时器5] 5秒后执行（这个应该被取消，不会执行）" << std::endl;
        }
    );
    
    if (timer5 > 0) {
        timer_ids.push_back(timer5);
        std::cout << "[主线程] 添加了定时器5 (ID=" << timer5 << ")，将在2秒后取消它" << std::endl;
        
        // 等待2秒后取消定时器
        std::this_thread::sleep_for(std::chrono::seconds(2));
        bool cancelled = global_timer.cancelTimer(timer5);
        std::cout << "[主线程] 取消定时器5: " << (cancelled ? "成功" : "失败") << std::endl;
    }
    
    // 等待一段时间让其他定时器完成
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    std::cout << "\n=== 优雅停止定时器 ===" << std::endl;
    global_timer.stop(true);  // true表示等待所有回调完成
    
    std::cout << "\n=== 程序退出 ===" << std::endl;
    return 0;
}