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
#include <algorithm>

// 改进的DualModeTimer类，支持优雅终止
class DualModeTimer {
public:
    using TimerCallback = std::function<void()>;
    
    // 定时器模式定义
    enum TimerMode {
        BLOCKING,    // 阻塞模式：定时期间阻塞线程
        NON_BLOCKING // 非阻塞模式：定时结束后回调通知
    };
    
    // 定时任务结构 - 只包含基础数据类型
    struct TimerTaskBase {
        int id;
        TimerMode mode;
        std::chrono::milliseconds duration;
        std::chrono::steady_clock::time_point start_time;
        std::chrono::steady_clock::time_point expiry_time;
        bool cancelled;
        
        // 用于优先级队列的比较函数
        bool operator<(const TimerTaskBase& other) const {
            return expiry_time > other.expiry_time; // 最小堆
        }
    };

private:
    // 包含回调的完整任务信息
    struct TimerTaskFull {
        TimerTaskBase base;
        TimerCallback callback;
        std::shared_ptr<std::promise<bool>> promise;
        std::atomic<bool> promise_set{false}; // 防止promise被多次设置
    };

    std::atomic<bool> running_{false};
    std::atomic<bool> shutdown_requested_{false};
    std::thread manager_thread_;
    std::mutex tasks_mutex_;
    std::condition_variable cv_;
    std::priority_queue<TimerTaskBase> tasks_queue_; // 只存储基础数据
    std::map<int, std::shared_ptr<TimerTaskFull>> active_tasks_; // 存储完整任务信息
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
        int task_id = addNonBlockingTimerImpl(duration, 
                                            [promise]() {
                                                try {
                                                    promise->set_value(true);
                                                } catch (const std::future_error& e) {
                                                    // 忽略promise已设置的情况
                                                }
                                            },
                                            promise);
        
        if (task_id < 0) {
            std::cerr << "[阻塞定时器] 创建失败" << std::endl;
            return false;
        }
        
        auto start_time = std::chrono::steady_clock::now();
        auto end_time = start_time + duration;
        
        // 分块等待，以便可以检查停止条件
        while (std::chrono::steady_clock::now() < end_time) {
            auto remaining = end_time - std::chrono::steady_clock::now();
            
            // 确保类型一致，使用相同的duration类型
            auto wait_time = std::min(remaining, 
                                     std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                                         std::chrono::milliseconds(100)));
            
            // 检查future状态
            auto status = future.wait_for(wait_time);
            if (status == std::future_status::ready) {
                // 定时器正常完成
                bool result = false;
                try {
                    result = future.get();
                } catch (...) {
                    result = false;
                }
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
            bool result = false;
            try {
                result = future.get();
            } catch (...) {
                result = false;
            }
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
        return addNonBlockingTimerImpl(duration, callback, nullptr);
    }
    
    // 取消定时器
    bool cancelTimer(int timer_id) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        auto it = active_tasks_.find(timer_id);
        if (it != active_tasks_.end()) {
            it->second->base.cancelled = true;
            
            // 如果有关联的promise，设置为false表示被取消
            if (it->second->promise && !it->second->promise_set.load()) {
                try {
                    it->second->promise_set.store(true);
                    it->second->promise->set_value(false);
                    std::cout << "[定时器] 取消: ID=" << timer_id << " (设置promise为false)" << std::endl;
                } catch (const std::future_error& e) {
                    // 忽略promise已设置的情况
                    std::cout << "[定时器] 取消时promise已设置: ID=" << timer_id << std::endl;
                }
            } else {
                std::cout << "[定时器] 取消: ID=" << timer_id << " (无promise或promise已设置)" << std::endl;
            }
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
            task->base.cancelled = true;
            
            if (task->promise && !task->promise_set.load()) {
                try {
                    task->promise_set.store(true);
                    task->promise->set_value(false);
                } catch (const std::future_error& e) {
                    // 忽略promise已设置的情况
                }
            }
        }
        
        // 清空任务队列
        tasks_queue_ = std::priority_queue<TimerTaskBase>();
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
    // 内部实现：添加非阻塞定时器
    int addNonBlockingTimerImpl(std::chrono::milliseconds duration, 
                               TimerCallback callback,
                               std::shared_ptr<std::promise<bool>> promise) {
        if (shutdown_requested_) {
            std::cerr << "[警告] 定时器正在关闭，无法添加新定时器" << std::endl;
            return -1;
        }
        
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        
        int id = ++next_id_;
        auto now = std::chrono::steady_clock::now();
        auto expiry = now + duration;
        
        // 创建基础任务信息
        TimerTaskBase base_task{
            id,
            NON_BLOCKING,
            duration,
            now,
            expiry,
            false
        };
        
        // 创建完整任务信息
        auto full_task = std::make_shared<TimerTaskFull>();
        full_task->base = base_task;
        full_task->callback = [this, id, callback]() {
            // 检查任务是否被取消
            bool cancelled = false;
            {
                std::lock_guard<std::mutex> lock(tasks_mutex_);
                auto it = active_tasks_.find(id);
                if (it != active_tasks_.end()) {
                    cancelled = it->second->base.cancelled;
                }
            }
            
            if (!cancelled) {
                std::cout << "[非阻塞定时器] 执行回调: ID=" << id << std::endl;
                callback();
            } else {
                std::cout << "[非阻塞定时器] 任务已取消，跳过回调: ID=" << id << std::endl;
            }
        };
        full_task->promise = promise;
        full_task->promise_set.store(false);
        
        // 添加到队列和活动任务列表
        tasks_queue_.push(base_task);
        active_tasks_[id] = full_task;
        cv_.notify_one();
        
        std::cout << "[非阻塞定时器] 添加: ID=" << id 
                  << ", 时长=" << duration.count() << "ms" << std::endl;
        
        return id;
    }
    
    // 管理器线程函数
    void managerThread() {
        while (running_ && !shutdown_requested_) {
            std::unique_lock<std::mutex> lock(tasks_mutex_);
            
            if (tasks_queue_.empty()) {
                // 等待新任务或关闭请求
                cv_.wait_for(lock, std::chrono::milliseconds(100));
                continue;
            }
            
            auto next_task_base = tasks_queue_.top();
            auto now = std::chrono::steady_clock::now();
            
            if (now >= next_task_base.expiry_time) {
                // 定时器到期
                tasks_queue_.pop();
                
                // 检查任务是否仍然有效且未取消
                auto it = active_tasks_.find(next_task_base.id);
                if (it != active_tasks_.end()) {
                    auto task = it->second;
                    
                    // 检查是否已取消
                    if (!task->base.cancelled) {
                        // 从活动任务中移除
                        active_tasks_.erase(it);
                        
                        // 执行回调（非阻塞定时器）
                        if (task->base.mode == NON_BLOCKING) {
                            // 在新线程中执行回调，避免阻塞管理器线程
                            std::lock_guard<std::mutex> cb_lock(callback_threads_mutex_);
                            callback_threads_.emplace_back([this, task]() {
                                try {
                                    std::cout << "[定时器] 触发: ID=" << task->base.id << std::endl;
                                    task->callback();
                                    
                                    // 如果有关联的promise，设置为true表示成功执行
                                    if (task->promise && !task->promise_set.load()) {
                                        try {
                                            task->promise_set.store(true);
                                            task->promise->set_value(true);
                                            std::cout << "[定时器] 设置promise为true: ID=" << task->base.id << std::endl;
                                        } catch (const std::future_error& e) {
                                            std::cerr << "[定时器] 设置promise异常: " << e.what() << " ID=" << task->base.id << std::endl;
                                        }
                                    }
                                } catch (const std::exception& e) {
                                    std::cerr << "[定时器] 回调异常: " << e.what() << " ID=" << task->base.id << std::endl;
                                    // 即使回调异常，也要设置promise
                                    if (task->promise && !task->promise_set.load()) {
                                        try {
                                            task->promise_set.store(true);
                                            task->promise->set_exception(std::current_exception());
                                        } catch (const std::future_error&) {
                                            // 忽略promise已设置的情况
                                        }
                                    }
                                }
                                
                                // 从线程列表中移除自己
                                {
                                    std::lock_guard<std::mutex> lock(callback_threads_mutex_);
                                    auto thread_id = std::this_thread::get_id();
                                    for (auto it = callback_threads_.begin(); it != callback_threads_.end(); ++it) {
                                        if (it->get_id() == thread_id) {
                                            if (it->joinable()) {
                                                it->detach();
                                            }
                                            callback_threads_.erase(it);
                                            break;
                                        }
                                    }
                                }
                                shutdown_cv_.notify_one();
                            });
                        }
                    } else {
                        // 任务已被取消，从活动任务中移除
                        std::cout << "[定时器] 跳过已取消的任务: ID=" << next_task_base.id << std::endl;
                        active_tasks_.erase(it);
                    }
                } else {
                    // 任务已被移除（可能已被取消）
                    std::cout << "[定时器] 任务已不存在，跳过: ID=" << next_task_base.id << std::endl;
                }
            } else {
                // 等待直到下一个定时器到期或收到关闭请求
                auto time_until_wake = next_task_base.expiry_time - now;
                auto wait_time = std::min(time_until_wake, 
                                         std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                                             std::chrono::milliseconds(100)));
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
            
            // 阻塞定时器，但可以响应停止请求
            bool success = global_timer.addBlockingTimer(
                std::chrono::milliseconds(3000),
                []() { 
                    // 检查是否应该停止
                    return should_exit.load() || global_timer.isShutdownRequested();
                }
            );
            
            std::cout << "[阻塞定时器线程] 结束，结果: " 
                      << (success ? "成功" : "失败/被取消") << std::endl;
        });
        
        // 等待一段时间
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // 模拟提前取消一些定时器
        std::cout << "\n--- 模拟取消部分定时器 ---" << std::endl;
        if (!timer_ids.empty()) {
            global_timer.cancelTimer(timer_ids[0]);
        }
        
        // 示例4: 模拟用户交互循环
        std::cout << "\n--- 主循环开始 (按Ctrl+C退出) ---" << std::endl;
        
        int counter = 0;
        while (!should_exit && counter < 10) {
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