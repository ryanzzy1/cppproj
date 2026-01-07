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
#include <random>
#include <csignal>
#include <future>

// 修改后的DualModeTimer类，添加了信号处理和更安全的线程管理
class DualModeTimer {
public:
    using TimerCallback = std::function<void()>;
    
    // 定时器模式定义
    enum TimerMode {
        BLOCKING,    // 阻塞模式：定时期间阻塞线程
        NON_BLOCKING // 非阻塞模式：定时结束后回调通知
    };
    
    // 定时任务结构
    struct TimerTask {
        int id;
        TimerMode mode;
        std::chrono::milliseconds duration;
        TimerCallback callback;
        std::chrono::steady_clock::time_point start_time;
        bool completed;
        std::shared_ptr<std::promise<bool>> promise; // 用于阻塞定时器的同步
        
        // 用于优先级队列的比较函数
        bool operator<(const TimerTask& other) const {
            return start_time + duration > other.start_time + other.duration;
        }
    };

private:
    std::atomic<bool> running_;
    std::thread manager_thread_;
    std::mutex tasks_mutex_;
    std::condition_variable cv_;
    std::priority_queue<TimerTask> tasks_;
    std::map<int, std::shared_ptr<TimerTask>> active_tasks_;
    std::atomic<int> next_id_;
    std::vector<std::thread> callback_threads_;
    std::mutex callback_threads_mutex_;

public:
    DualModeTimer() : running_(false), next_id_(0) {}
    
    ~DualModeTimer() {
        stop();
    }

    // 启动定时器管理器
    void start() {
        if (running_) return;
        
        running_ = true;
        manager_thread_ = std::thread(&DualModeTimer::managerThread, this);
        std::cout << "[定时器管理器] 已启动" << std::endl;
    }
    
    // 停止定时器管理器
    void stop() {
        if (!running_) return;
        
        std::cout << "[定时器管理器] 正在停止..." << std::endl;
        
        // 首先设置停止标志
        running_ = false;
        
        // 取消所有定时器
        cancelAllTimers();
        
        // 通知所有等待的线程
        cv_.notify_all();
        
        // 等待管理器线程结束
        if (manager_thread_.joinable()) {
            manager_thread_.join();
        }
        
        // 等待所有回调线程完成
        {
            std::lock_guard<std::mutex> lock(callback_threads_mutex_);
            for (auto& thread : callback_threads_) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
            callback_threads_.clear();
        }
        
        std::cout << "[定时器管理器] 已完全停止" << std::endl;
    }
    
    // 添加阻塞定时器：调用线程会被阻塞直到定时结束
    bool addBlockingTimer(std::chrono::milliseconds duration) {
        std::cout << "[阻塞定时器] 开始: " << duration.count() << "ms" << std::endl;
        
        auto promise = std::make_shared<std::promise<bool>>();
        auto future = promise->get_future();
        bool timer_expired = false;
        
        // 创建非阻塞定时器来驱动阻塞定时
        int task_id = addNonBlockingTimer(duration, [promise, &timer_expired]() {
            timer_expired = true;
            promise->set_value(true);
        });
        
        if (task_id < 0) {
            std::cerr << "[阻塞定时器] 创建失败" << std::endl;
            return false;
        }
        
        // 等待定时结束或被取消
        auto status = future.wait_for(duration + std::chrono::milliseconds(100));
        
        // 检查是否超时
        if (status == std::future_status::timeout) {
            std::cout << "[阻塞定时器] 等待超时，正在取消..." << std::endl;
            cancelTimer(task_id);
            return false;
        }
        
        // 检查是否被取消
        bool result = future.get();
        if (!result) {
            std::cout << "[阻塞定时器] 被取消" << std::endl;
            return false;
        }
        
        std::cout << "[阻塞定时器] 结束: ID=" << task_id << std::endl;
        return true;
    }
    
    // 添加非阻塞定时器：定时结束后通过回调函数通知
    int addNonBlockingTimer(std::chrono::milliseconds duration, TimerCallback callback) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        
        int id = ++next_id_;
        TimerTask task{
            id,
            NON_BLOCKING,
            duration,
            callback,
            std::chrono::steady_clock::now(),
            false,
            nullptr
        };
        
        tasks_.push(task);
        active_tasks_[id] = std::make_shared<TimerTask>(task);
        cv_.notify_one();
        
        std::cout << "[非阻塞定时器] 添加: ID=" << id 
                  << ", 时长=" << duration.count() << "ms" << std::endl;
        
        return id;
    }
    
    // 添加带promise的非阻塞定时器（内部使用）
    int addNonBlockingTimerWithPromise(std::chrono::milliseconds duration, 
                                      std::shared_ptr<std::promise<bool>> promise,
                                      TimerCallback callback) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        
        int id = ++next_id_;
        TimerTask task{
            id,
            NON_BLOCKING,
            duration,
            callback,
            std::chrono::steady_clock::now(),
            false,
            promise
        };
        
        tasks_.push(task);
        active_tasks_[id] = std::make_shared<TimerTask>(task);
        cv_.notify_one();
        
        return id;
    }
    
    // 取消定时器
    bool cancelTimer(int timer_id) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        auto it = active_tasks_.find(timer_id);
        if (it != active_tasks_.end()) {
            it->second->completed = true;
            
            // 如果有关联的promise，设置为false表示被取消
            if (it->second->promise) {
                it->second->promise->set_value(false);
            }
            
            active_tasks_.erase(it);
            std::cout << "[定时器] 取消: ID=" << timer_id << std::endl;
            return true;
        }
        return false;
    }
    
    // 取消所有定时器
    void cancelAllTimers() {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        std::cout << "[定时器管理器] 正在取消所有定时器 (" 
                  << active_tasks_.size() << " 个活动定时器)" << std::endl;
        
        for (auto& pair : active_tasks_) {
            auto& task = pair.second;
            task->completed = true;
            
            if (task->promise) {
                try {
                    task->promise->set_value(false);
                } catch (const std::future_error& e) {
                    // 忽略promise已设置的情况
                }
            }
        }
        
        // 清空任务队列
        tasks_ = std::priority_queue<TimerTask>();
        active_tasks_.clear();
    }
    
    // 获取活动定时器数量
    int getActiveTimerCount() {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        return active_tasks_.size();
    }
    
    // 检查是否正在运行
    bool isRunning() const {
        return running_;
    }

private:
    // 管理器线程函数
    void managerThread() {
        while (running_) {
            std::unique_lock<std::mutex> lock(tasks_mutex_);
            
            if (tasks_.empty()) {
                cv_.wait(lock);
                continue;
            }
            
            auto next_task = tasks_.top();
            auto now = std::chrono::steady_clock::now();
            auto wake_time = next_task.start_time + next_task.duration;
            
            if (now >= wake_time) {
                // 定时器到期
                tasks_.pop();
                
                // 检查任务是否仍然有效且未完成
                auto it = active_tasks_.find(next_task.id);
                if (it != active_tasks_.end() && !it->second->completed) {
                    auto task = it->second;
                    active_tasks_.erase(it);
                    
                    // 执行回调（非阻塞定时器）
                    if (task->mode == NON_BLOCKING) {
                        // 在新线程中执行回调，避免阻塞管理器线程
                        std::lock_guard<std::mutex> cb_lock(callback_threads_mutex_);
                        callback_threads_.emplace_back([task]() {
                            try {
                                std::cout << "[定时器] 触发: ID=" << task->id << std::endl;
                                task->callback();
                                
                                // 如果有关联的promise，设置为true表示成功执行
                                if (task->promise) {
                                    task->promise->set_value(true);
                                }
                            } catch (const std::exception& e) {
                                std::cerr << "[定时器] 回调异常: " << e.what() << std::endl;
                                if (task->promise) {
                                    task->promise->set_exception(std::current_exception());
                                }
                            }
                        });
                    }
                }
            } else {
                // 等待直到下一个定时器到期
                cv_.wait_until(lock, wake_time);
            }
        }
        
        // 管理器线程结束前清理回调线程
        std::lock_guard<std::mutex> cb_lock(callback_threads_mutex_);
        for (auto& thread : callback_threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        callback_threads_.clear();
    }
};

// 全局定时器实例和停止标志
DualModeTimer global_timer;
std::atomic<bool> should_exit{false};

// 信号处理函数
void signal_handler(int signal) {
    std::cout << "\n[系统] 收到信号 " << signal << "，正在优雅退出..." << std::endl;
    should_exit = true;
}

// 模拟工作函数
void simulate_work(const std::string& name, int duration_ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    std::cout << "[工作完成] " << name << " 耗时 " << duration_ms << "ms" << std::endl;
}

// 非阻塞定时器回调示例
void timer_callback_1() {
    simulate_work("定时器任务1", 200);
}

void timer_callback_2(int id) {
    std::cout << "[回调] 定时器 " << id << " 执行任务" << std::endl;
    simulate_work("定时器任务2", 300);
}

void timer_callback_periodic(int count) {
    static int call_count = 0;
    call_count++;
    std::cout << "[周期性回调] 第 " << call_count << " 次调用 (参数: " << count << ")" << std::endl;
    
    if (call_count >= 5) {
        std::cout << "[周期性回调] 已达到最大调用次数，将自动取消" << std::endl;
    }
}

// 主函数
int main() {
    std::cout << "=== 可随时终止的定时器示例 ===" << std::endl;
    std::cout << "按 Ctrl+C 可随时终止程序" << std::endl;
    
    // 设置信号处理
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    // 启动定时器管理器
    global_timer.start();
    
    std::vector<int> timer_ids;
    
    try {
        // 示例1: 添加几个非阻塞定时器
        std::cout << "\n--- 添加非阻塞定时器 ---" << std::endl;
        
        int timer1 = global_timer.addNonBlockingTimer(
            std::chrono::milliseconds(1000), 
            timer_callback_1
        );
        timer_ids.push_back(timer1);
        
        int timer2 = global_timer.addNonBlockingTimer(
            std::chrono::milliseconds(2000),
            std::bind(timer_callback_2, 100)
        );
        timer_ids.push_back(timer2);
        
        // 示例2: 添加一个长时间的非阻塞定时器
        int timer3 = global_timer.addNonBlockingTimer(
            std::chrono::milliseconds(5000),
            []() {
                std::cout << "[长时间任务] 开始执行..." << std::endl;
                simulate_work("长时间任务", 2000);
                std::cout << "[长时间任务] 执行完成" << std::endl;
            }
        );
        timer_ids.push_back(timer3);
        
        // 示例3: 在主线程中执行阻塞定时器
        std::cout << "\n--- 执行阻塞定时器 ---" << std::endl;
        
        std::thread blocking_thread([]() {
            std::cout << "[阻塞定时器线程] 开始" << std::endl;
            bool success = global_timer.addBlockingTimer(std::chrono::milliseconds(3000));
            std::cout << "[阻塞定时器线程] 结束，结果: " 
                      << (success ? "成功" : "失败/被取消") << std::endl;
        });
        
        // 示例4: 模拟用户交互循环
        std::cout << "\n--- 主循环开始 (按Ctrl+C退出) ---" << std::endl;
        
        int counter = 0;
        while (!should_exit && counter < 20) {
            std::cout << "[主循环] 迭代 " << ++counter 
                      << "，活动定时器: " << global_timer.getActiveTimerCount() << std::endl;
            
            // 随机添加一些新的定时器
            if (counter % 3 == 0 && !should_exit) {
                int new_timer = global_timer.addNonBlockingTimer(
                    std::chrono::milliseconds(1000 + (counter * 100)),
                    std::bind(timer_callback_periodic, counter)
                );
                timer_ids.push_back(new_timer);
            }
            
            // 随机取消一些定时器
            if (counter % 4 == 0 && !timer_ids.empty() && !should_exit) {
                int index = counter % timer_ids.size();
                global_timer.cancelTimer(timer_ids[index]);
                std::cout << "[主循环] 取消了定时器 ID=" << timer_ids[index] << std::endl;
            }
            
            // 等待一段时间
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            // 模拟外部事件导致退出
            if (counter == 10) {
                std::cout << "\n[模拟] 触发紧急停止条件！" << std::endl;
                // 这里可以取消所有定时器并退出
                // should_exit = true;
            }
        }
        
        // 等待阻塞定时器线程完成
        if (blocking_thread.joinable()) {
            blocking_thread.join();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "\n[异常] " << e.what() << std::endl;
    }
    
    // 优雅停止定时器
    std::cout << "\n--- 正在停止定时器管理器 ---" << std::endl;
    global_timer.stop();
    
    std::cout << "\n=== 程序正常退出 ===" << std::endl;
    return 0;
}