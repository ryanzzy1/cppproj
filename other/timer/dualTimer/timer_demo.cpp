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

// 全局停止标志
std::atomic<bool> should_exit{false};

// 信号处理函数
void signal_handler(int signal) {
    std::cout << "\n[系统] 收到信号 " << signal << "，正在优雅退出..." << std::endl;
    should_exit = true;
}

// 可中断的阻塞等待函数
bool interruptible_sleep(std::chrono::milliseconds duration, const std::function<bool()>& interrupt_check = nullptr) {
    using SteadyDuration = std::chrono::steady_clock::duration;
    
    auto start = std::chrono::steady_clock::now();
    auto end = start + duration;
    
    while (std::chrono::steady_clock::now() < end) {
        auto remaining = end - std::chrono::steady_clock::now();
        auto wait_time = std::min<SteadyDuration>(
            remaining, 
            std::chrono::duration_cast<SteadyDuration>(std::chrono::milliseconds(100)));
        
        std::this_thread::sleep_for(wait_time);
        
        // 检查是否应该中断
        if (should_exit.load()) {
            std::cout << "[中断] 收到退出信号，停止等待" << std::endl;
            return false;
        }
        
        // 检查自定义中断条件
        if (interrupt_check && interrupt_check()) {
            std::cout << "[中断] 满足自定义中断条件，停止等待" << std::endl;
            return false;
        }
    }
    
    return true;
}

// 简化版定时器类，专注于解决问题
class SimpleTimer {
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
    std::condition_variable cv_;
    std::priority_queue<TimerTask> tasks_queue_;
    std::map<int, TimerTask> active_tasks_;
    std::atomic<int> next_id_{0};

public:
    SimpleTimer() = default;
    
    ~SimpleTimer() {
        stop();
    }

    void start() {
        if (running_) return;
        
        running_ = true;
        manager_thread_ = std::thread(&SimpleTimer::managerThread, this);
        std::cout << "[定时器管理器] 已启动" << std::endl;
    }
    
    void stop() {
        if (!running_) return;
        
        std::cout << "[定时器管理器] 正在停止..." << std::endl;
        running_ = false;
        cv_.notify_all();
        
        if (manager_thread_.joinable()) {
            manager_thread_.join();
        }
        
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        tasks_queue_ = std::priority_queue<TimerTask>();
        active_tasks_.clear();
        std::cout << "[定时器管理器] 已完全停止" << std::endl;
    }
    
    // 阻塞定时器：支持中断的阻塞定时器
    bool addBlockingTimer(std::chrono::milliseconds duration, 
                         const std::function<bool()>& should_stop = nullptr) {
        std::cout << "[阻塞定时器] 开始: " << duration.count() << "ms" << std::endl;
        
        // 使用可中断的睡眠
        bool completed = interruptible_sleep(duration, [should_stop]() {
            if (should_stop) {
                return should_stop();
            }
            return false;
        });
        
        if (completed) {
            std::cout << "[阻塞定时器] 结束" << std::endl;
            return true;
        } else {
            std::cout << "[阻塞定时器] 被中断" << std::endl;
            return false;
        }
    }
    
    // 非阻塞定时器：支持中断回调
    int addNonBlockingTimer(std::chrono::milliseconds duration, 
                           TimerCallback callback,
                           const std::function<bool()>& should_stop = nullptr) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        
        // 检查是否应该退出
        if (should_exit.load()) {
            std::cout << "[警告] 程序正在退出，不添加新定时器" << std::endl;
            return -1;
        }
        
        int id = ++next_id_;
        auto now = std::chrono::steady_clock::now();
        auto expiry = now + duration;
        
        // 创建可中断的回调函数
        auto interruptible_callback = [callback, should_stop]() {
            // 检查是否应该退出
            if (should_exit.load()) {
                std::cout << "[中断] 收到退出信号，跳过回调执行" << std::endl;
                return;
            }
            
            try {
                std::cout << "[非阻塞定时器] 执行回调" << std::endl;
                
                // 如果回调需要检查中断条件，可以在这里实现
                // 例如，如果回调是一个长时间运行的任务，它可以定期检查should_exit
                callback();
            } catch (const std::exception& e) {
                std::cerr << "[非阻塞定时器] 回调异常: " << e.what() << std::endl;
            }
        };
        
        TimerTask task{
            id,
            expiry,
            interruptible_callback,
            false
        };
        
        tasks_queue_.push(task);
        active_tasks_[id] = task;
        cv_.notify_one();
        
        std::cout << "[非阻塞定时器] 添加: ID=" << id 
                  << ", 时长=" << duration.count() << "ms" << std::endl;
        
        return id;
    }
    
    bool cancelTimer(int timer_id) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        auto it = active_tasks_.find(timer_id);
        if (it != active_tasks_.end()) {
            it->second.is_cancelled = true;
            std::cout << "[定时器] 取消: ID=" << timer_id << std::endl;
            return true;
        }
        return false;
    }
    
    // 取消所有定时器
    void cancelAllTimers() {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        std::cout << "[定时器管理器] 取消所有定时器 (" 
                  << active_tasks_.size() << " 个活动定时器)" << std::endl;
        
        for (auto& pair : active_tasks_) {
            pair.second.is_cancelled = true;
        }
    }
    
    int getActiveTimerCount() {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        return active_tasks_.size();
    }

private:
    void managerThread() {
        while (running_) {
            std::unique_lock<std::mutex> lock(tasks_mutex_);
            
            if (tasks_queue_.empty()) {
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
                        // 执行回调，避免阻塞管理器线程
                        std::thread([task]() {
                            // 在回调执行前检查退出标志
                            if (should_exit.load()) {
                                std::cout << "[定时器] 程序正在退出，跳过执行: ID=" << task.id << std::endl;
                                return;
                            }
                            
                            try {
                                std::cout << "[定时器] 触发: ID=" << task.id << std::endl;
                                task.callback();
                            } catch (const std::exception& e) {
                                std::cerr << "[定时器] 回调异常: " << e.what() << std::endl;
                            }
                        }).detach();
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
        
        std::cout << "[定时器管理器] 管理器线程退出" << std::endl;
    }
};

// 全局定时器实例
SimpleTimer global_timer;

// 可中断的工作函数
void interruptible_work(const std::string& name, 
                       std::chrono::milliseconds duration,
                       const std::function<bool()>& interrupt_check = nullptr) {
    using SteadyDuration = std::chrono::steady_clock::duration;
    
    auto start = std::chrono::steady_clock::now();
    auto end = start + duration;
    
    int step = 0;
    while (std::chrono::steady_clock::now() < end) {
        step++;
        std::cout << "[" << name << "] 步骤 " << step << std::endl;
        
        auto remaining = end - std::chrono::steady_clock::now();
        auto step_duration = std::min<SteadyDuration>(
            remaining, 
            std::chrono::duration_cast<SteadyDuration>(
                std::chrono::milliseconds(1000)));
        
        // 使用可中断的睡眠
        if (!interruptible_sleep(
                std::chrono::duration_cast<std::chrono::milliseconds>(step_duration), 
                interrupt_check)) {
            std::cout << "[" << name << "] 被中断" << std::endl;
            return;
        }
    }
    
    std::cout << "[" << name << "] 完成" << std::endl;
}

int main() {
    std::cout << "=== 可随时终止的定时器示例 ===" << std::endl;
    std::cout << "按 Ctrl+C 可随时终止程序" << std::endl;
    
    // 设置信号处理
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    // 启动定时器管理器
    global_timer.start();
    
    std::vector<int> timer_ids;
    
    std::cout << "\n[BLOCK]---阻塞定时器开始5s计时: ---" << std::endl;

    // 阻塞定时器 - 可以被中断
    bool block_result = global_timer.addBlockingTimer(std::chrono::milliseconds(5000));
    
    std::cout << "\n[BLOCK] 阻塞定时器结果: " << (block_result ? "成功" : "被中断") << std::endl;
    std::cout << "\n[BLOCK]--- 阻塞定时器结束 ---" << std::endl;

    // 重置退出标志，以便继续测试
    should_exit.store(false);
    
    std::cout << "\n[NONBLOCK]---非阻塞定时器开始5s计时: ---" << std::endl;

    // 非阻塞定时器 - 使用可中断的回调
    int timer1 = global_timer.addNonBlockingTimer(
        std::chrono::milliseconds(3000),  // 改为3秒，确保有时间执行
        []() {
            std::cout << "[NONBLOCK][非阻塞定时器] 3秒后回调执行" << std::endl;
            
            // 使用可中断的工作函数
            interruptible_work("非阻塞定时器任务", 
                              std::chrono::milliseconds(4000),
                              []() { return should_exit.load(); });
            
            std::cout << "[NONBLOCK][非阻塞定时器] 回调执行完成或中断" << std::endl;
        }
    );
    
    if(timer1 > 0) {
        timer_ids.push_back(timer1);
        std::cout << "[NONBLOCK] 非阻塞定时器添加成功: ID=" << timer1 << std::endl;
    } else {
        std::cout << "[NONBLOCK] 非阻塞定时器添加失败" << std::endl;
    }
    
    std::cout << "\n[NONBLOCK]--- 非阻塞定时器已添加，等待2秒后发送中断信号 ---" << std::endl;
    
    // 等待2秒，让定时器有机会触发
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "\n[NONBLOCK] 已等待2秒，还有1秒定时器将触发..." << std::endl;
    
    // 再等待1秒，让定时器触发
    std::this_thread::sleep_for(std::chrono::seconds(1));
    /*
    std::cout << "\n[NONBLOCK] 发送中断信号..." << std::endl;
    should_exit.store(true);
    
    // 等待一段时间让定时器响应中断
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "\n[NONBLOCK]--- 非阻塞定时器应该已经被中断 ---" << std::endl;*/

    // 优雅停止定时器
    std::cout << "\n--- 正在停止定时器管理器 ---" << std::endl;
    global_timer.stop();
    
    std::cout << "\n=== 程序退出 ===" << std::endl;
    return 0;
}