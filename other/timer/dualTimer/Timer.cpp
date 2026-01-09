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

// 改进的DualModeTimer类，支持优雅终止
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
        std::atomic<bool> cancelled{false};
        std::shared_ptr<std::promise<bool>> promise; // 用于阻塞定时器的同步
        
        // 用于优先级队列的比较函数
        bool operator<(const TimerTask& other) const {
            return start_time + duration > other.start_time + other.duration;
        }
    };

private:
    std::atomic<bool> running_{false};
    std::atomic<bool> shutdown_requested_{false};
    std::thread manager_thread_;
    std::mutex tasks_mutex_;
    std::condition_variable cv_;
    std::priority_queue<TimerTask> tasks_;
    std::map<int, std::shared_ptr<TimerTask>> active_tasks_;
    std::atomic<int> next_id_{0};
    std::vector<std::thread> callback_threads_;
    std::mutex callback_threads_mutex_;
    std::condition_variable shutdown_cv_;

public:
    DualModeTimer() = default;
    
    ~DualModeTimer() {
        stop();
    }

    // 启动定时器管理器
    void start() {
        if (running_) return;
        
        running_ = true;
        shutdown_requested_ = false;
        manager_thread_ = std::thread(&DualModeTimer::managerThread, this);
        std::cout << "[定时器管理器] 已启动" << std::endl;
    }
    
    // 停止定时器管理器（优雅停止）
    void stop() {
        if (!running_) return;
        
        std::cout << "[定时器管理器] 正在停止..." << std::endl;
        
        // 请求关闭
        shutdown_requested_ = true;
        
        // 取消所有定时器
        cancelAllTimers();
        
        // 通知所有等待的线程
        cv_.notify_all();
        shutdown_cv_.notify_all();
        
        // 等待管理器线程结束
        if (manager_thread_.joinable()) {
            manager_thread_.join();
        }
        
        // 等待所有回调线程完成（最多等待1秒）
        {
            std::unique_lock<std::mutex> lock(callback_threads_mutex_);
            auto now = std::chrono::steady_clock::now();
            auto deadline = now + std::chrono::seconds(1);
            
            // 等待所有回调线程完成
            while (!callback_threads_.empty()) {
                if (shutdown_cv_.wait_until(lock, deadline) == std::cv_status::timeout) {
                    std::cout << "[定时器管理器] 等待回调线程超时，强制终止" << std::endl;
                    break;
                }
            }
            
            // 强制分离未完成的线程
            for (auto& thread : callback_threads_) {
                if (thread.joinable()) {
                    thread.detach();
                }
            }
            callback_threads_.clear();
        }
        
        running_ = false;
        std::cout << "[定时器管理器] 已完全停止" << std::endl;
    }
    
    // 添加阻塞定时器：调用线程会被阻塞直到定时结束或被中断
    bool addBlockingTimer(std::chrono::milliseconds duration, 
                         const std::function<bool()>& should_stop = nullptr) {
        std::cout << "[阻塞定时器] 开始: " << duration.count() << "ms" << std::endl;
        
        auto promise = std::make_shared<std::promise<bool>>();
        auto future = promise->get_future();
        
        // 创建非阻塞定时器来驱动阻塞定时
        int task_id = addNonBlockingTimer(duration, [promise]() {
            promise->set_value(true);
        });
        
        if (task_id < 0) {
            std::cerr << "[阻塞定时器] 创建失败" << std::endl;
            return false;
        }
        
        auto start_time = std::chrono::steady_clock::now();
        auto end_time = start_time + duration;
        
        // 分块等待，以便可以检查停止条件
        while (std::chrono::steady_clock::now() < end_time) {
            auto remaining = end_time - std::chrono::steady_clock::now();
            auto wait_time = std::min(remaining, std::chrono::milliseconds(100));
            
            // 检查future状态
            auto status = future.wait_for(wait_time);
            if (status == std::future_status::ready) {
                // 定时器正常完成
                bool result = future.get();
                std::cout << "[阻塞定时器] 结束: ID=" << task_id 
                          << (result ? " (成功)" : " (被取消)") << std::endl;
                return result;
            }
            
            // 检查是否应该停止
            if (shutdown_requested_) {
                std::cout << "[阻塞定时器] 收到关闭请求，正在取消..." << std::endl;
                cancelTimer(task_id);
                return false;
            }
            
            // 检查外部停止条件
            if (should_stop && should_stop()) {
                std::cout << "[阻塞定时器] 外部条件触发停止，正在取消..." << std::endl;
                cancelTimer(task_id);
                return false;
            }
        }
        
        // 等待最后一点时间
        auto status = future.wait_for(std::chrono::milliseconds(100));
        if (status == std::future_status::ready) {
            bool result = future.get();
            std::cout << "[阻塞定时器] 结束: ID=" << task_id 
                      << (result ? " (成功)" : " (被取消)") << std::endl;
            return result;
        }
        
        // 超时
        std::cout << "[阻塞定时器] 超时，强制取消" << std::endl;
        cancelTimer(task_id);
        return false;
    }
    
    // 添加非阻塞定时器：定时结束后通过回调函数通知
    int addNonBlockingTimer(std::chrono::milliseconds duration, TimerCallback callback) {
        if (shutdown_requested_) {
            std::cerr << "[警告] 定时器正在关闭，无法添加新定时器" << std::endl;
            return -1;
        }
        
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        
        int id = ++next_id_;
        auto task_ptr = std::make_shared<TimerTask>();
        task_ptr->id = id;
        task_ptr->mode = NON_BLOCKING;
        task_ptr->duration = duration;
        task_ptr->callback = [this, id, callback]() {
            if (!active_tasks_[id]->cancelled.load()) {
                std::cout << "[非阻塞定时器] 执行回调: ID=" << id << std::endl;
                callback();
            }
        };
        task_ptr->start_time = std::chrono::steady_clock::now();
        
        tasks_.push(*task_ptr);
        active_tasks_[id] = task_ptr;
        cv_.notify_one();
        
        std::cout << "[非阻塞定时器] 添加: ID=" << id 
                  << ", 时长=" << duration.count() << "ms" << std::endl;
        
        return id;
    }
    
    // 取消定时器
    bool cancelTimer(int timer_id) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        auto it = active_tasks_.find(timer_id);
        if (it != active_tasks_.end()) {
            it->second->cancelled.store(true);
            
            // 如果有关联的promise，设置为false表示被取消
            if (it->second->promise) {
                try {
                    it->second->promise->set_value(false);
                } catch (const std::future_error& e) {
                    // 忽略promise已设置的情况
                }
            }
            
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
            task->cancelled.store(true);
            
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
    
    // 检查是否请求关闭
    bool isShutdownRequested() const {
        return shutdown_requested_;
    }

private:
    // 管理器线程函数
    void managerThread() {
        while (running_ && !shutdown_requested_) {
            std::unique_lock<std::mutex> lock(tasks_mutex_);
            
            if (tasks_.empty()) {
                // 等待新任务或关闭请求
                cv_.wait_for(lock, std::chrono::milliseconds(100));
                continue;
            }
            
            auto next_task = tasks_.top();
            auto now = std::chrono::steady_clock::now();
            auto wake_time = next_task.start_time + next_task.duration;
            
            if (now >= wake_time) {
                // 定时器到期
                tasks_.pop();
                
                // 检查任务是否仍然有效且未取消
                auto it = active_tasks_.find(next_task.id);
                if (it != active_tasks_.end() && !it->second->cancelled.load()) {
                    auto task = it->second;
                    active_tasks_.erase(it);
                    
                    // 执行回调（非阻塞定时器）
                    if (task->mode == NON_BLOCKING) {
                        // 在新线程中执行回调，避免阻塞管理器线程
                        std::lock_guard<std::mutex> cb_lock(callback_threads_mutex_);
                        callback_threads_.emplace_back([this, task]() {
                            try {
                                task->callback();
                            } catch (const std::exception& e) {
                                std::cerr << "[定时器] 回调异常: " << e.what() << std::endl;
                            }
                            
                            // 从线程列表中移除自己
                            {
                                std::lock_guard<std::mutex> lock(callback_threads_mutex_);
                                auto it = std::find_if(callback_threads_.begin(), callback_threads_.end(),
                                                      [](const std::thread& t) { return t.get_id() == std::this_thread::get_id(); });
                                if (it != callback_threads_.end()) {
                                    it->detach(); // 先分离
                                    callback_threads_.erase(it);
                                }
                            }
                            shutdown_cv_.notify_one();
                        });
                    }
                } else if (it != active_tasks_.end()) {
                    // 任务已被取消，从活动任务中移除
                    active_tasks_.erase(it);
                }
            } else {
                // 等待直到下一个定时器到期或收到关闭请求
                auto wait_time = std::min(wake_time - now, std::chrono::milliseconds(100));
                cv_.wait_for(lock, wait_time);
            }
        }
        
        // 管理器线程结束前清理回调线程
        std::lock_guard<std::mutex> cb_lock(callback_threads_mutex_);
        for (auto& thread : callback_threads_) {
            if (thread.joinable()) {
                thread.detach();
            }
        }
        callback_threads_.clear();
    }
};

// 全局定时器实例和停止标志
DualModeTimer global_timer;
std::atomic<bool> should_exit{false};
std::atomic<bool> blocking_timer_running{false};

// 信号处理函数
void signal_handler(int signal) {
    std::cout << "\n[系统] 收到信号 " << signal << "，正在优雅退出..." << std::endl;
    should_exit = true;
}

// 模拟工作函数（可检查停止条件）
void simulate_work_with_check(const std::string& name, int duration_ms, 
                             const std::function<bool()>& should_stop = nullptr) {
    auto start = std::chrono::steady_clock::now();
    auto end = start + std::chrono::milliseconds(duration_ms);
    
    while (std::chrono::steady_clock::now() < end) {
        // 检查是否应该停止
        if (should_stop && should_stop()) {
            std::cout << "[工作] " << name << " 被中断" << std::endl;
            return;
        }
        
        // 工作一小段时间
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::cout << "[工作完成] " << name << " 耗时 " << duration_ms << "ms" << std::endl;
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
            []() {
                simulate_work_with_check("定时器任务1", 200, []() { 
                    return should_exit.load(); 
                });
            }
        );
        if (timer1 > 0) timer_ids.push_back(timer1);
        
        int timer2 = global_timer.addNonBlockingTimer(
            std::chrono::milliseconds(2000),
            []() {
                std::cout << "[回调] 定时器任务2 执行" << std::endl;
                simulate_work_with_check("定时器任务2", 300, []() { 
                    return should_exit.load(); 
                });
            }
        );
        if (timer2 > 0) timer_ids.push_back(timer2);
        
        // 示例2: 添加一个长时间的非阻塞定时器
        int timer3 = global_timer.addNonBlockingTimer(
            std::chrono::milliseconds(5000),
            []() {
                std::cout << "[长时间任务] 开始执行..." << std::endl;
                simulate_work_with_check("长时间任务", 2000, []() { 
                    return should_exit.load(); 
                });
                std::cout << "[长时间任务] 执行完成" << std::endl;
            }
        );
        if (timer3 > 0) timer_ids.push_back(timer3);
        
        // 示例3: 在主线程中执行阻塞定时器（可中断）
        std::cout << "\n--- 执行可中断的阻塞定时器 ---" << std::endl;
        
        std::thread blocking_thread([]() {
            std::cout << "[阻塞定时器线程] 开始" << std::endl;
            blocking_timer_running = true;
            
            // 阻塞定时器，但可以响应停止请求
            bool success = global_timer.addBlockingTimer(
                std::chrono::milliseconds(3000),
                []() { 
                    // 检查是否应该停止
                    return should_exit.load() || global_timer.isShutdownRequested();
                }
            );
            
            blocking_timer_running = false;
            std::cout << "[阻塞定时器线程] 结束，结果: " 
                      << (success ? "成功" : "失败/被取消") << std::endl;
        });
        
        // 示例4: 模拟用户交互循环
        std::cout << "\n--- 主循环开始 (按Ctrl+C退出) ---" << std::endl;
        
        int counter = 0;
        while (!should_exit && counter < 20) {
            std::cout << "[主循环] 迭代 " << ++counter 
                      << "，活动定时器: " << global_timer.getActiveTimerCount() 
                      << "，退出标志: " << should_exit.load() << std::endl;
            
            // 随机添加一些新的定时器
            if (counter % 3 == 0 && !should_exit && !global_timer.isShutdownRequested()) {
                int new_timer = global_timer.addNonBlockingTimer(
                    std::chrono::milliseconds(1000 + (counter * 100)),
                    [counter]() {
                        static int call_count = 0;
                        call_count++;
                        std::cout << "[周期性回调] 第 " << call_count << " 次调用 (参数: " << counter << ")" << std::endl;
                    }
                );
                if (new_timer > 0) timer_ids.push_back(new_timer);
            }
            
            // 随机取消一些定时器
            if (counter % 4 == 0 && !timer_ids.empty() && !should_exit) {
                int index = counter % timer_ids.size();
                global_timer.cancelTimer(timer_ids[index]);
                std::cout << "[主循环] 取消了定时器 ID=" << timer_ids[index] << std::endl;
            }
            
            // 等待一段时间，但可以响应停止请求
            auto start_wait = std::chrono::steady_clock::now();
            auto wait_duration = std::chrono::milliseconds(500);
            
            while (std::chrono::steady_clock::now() < start_wait + wait_duration) {
                if (should_exit.load()) break;
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            
            // 模拟外部事件导致退出
            if (counter == 10) {
                std::cout << "\n[模拟] 触发紧急停止条件！" << std::endl;
                should_exit = true;
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
    
    std::cout << "\n=== 程序退出 ===" << std::endl;
    return 0;
}