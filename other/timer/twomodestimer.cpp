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
    int next_id_;

public:
    DualModeTimer() : running_(false), next_id_(0) {}
    
    ~DualModeTimer() {
        stop();
    }

    // 启动定时器管理器
    void start() {
        running_ = true;
        manager_thread_ = std::thread(&DualModeTimer::managerThread, this);
    }
    
    // 停止定时器管理器
    void stop() {
        running_ = false;
        cv_.notify_all();
        if (manager_thread_.joinable()) {
            manager_thread_.join();
        }
    }
    
    // 添加阻塞定时器：调用线程会被阻塞直到定时结束
    void addBlockingTimer(std::chrono::milliseconds duration) {
        std::cout << "阻塞定时器开始: " << duration.count() << "ms" << std::endl;
        
        // 使用条件变量实现精确的阻塞等待
        std::mutex local_mutex;
        std::unique_lock<std::mutex> lock(local_mutex);
        std::condition_variable local_cv;
        bool finished = false;
        
        // 在管理器中注册一个非阻塞任务来通知本地条件变量
        int task_id = addNonBlockingTimer(duration, [&]() {
            std::lock_guard<std::mutex> guard(local_mutex);
            finished = true;
            local_cv.notify_one();
        });
        
        // 阻塞当前线程，等待定时结束
        local_cv.wait(lock, [&]() { return finished; });
        
        std::cout << "阻塞定时器结束" << std::endl;
    }
    
    // 添加非阻塞定时器：定时结束后通过回调函数通知
    int addNonBlockingTimer(std::chrono::milliseconds duration, TimerCallback callback) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        
        TimerTask task{
            next_id_,
            NON_BLOCKING,
            duration,
            callback,
            std::chrono::steady_clock::now(),
            false
        };
        
        tasks_.push(task);
        active_tasks_[next_id_] = std::make_shared<TimerTask>(task);
        cv_.notify_one();
        
        std::cout << "非阻塞定时器添加: ID=" << next_id_ 
                  << ", 时长=" << duration.count() << "ms" << std::endl;
        
        return next_id_++;
    }
    
    // 取消定时器
    bool cancelTimer(int timer_id) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        auto it = active_tasks_.find(timer_id);
        if (it != active_tasks_.end()) {
            it->second->completed = true;
            active_tasks_.erase(it);
            std::cout << "定时器取消: ID=" << timer_id << std::endl;
            return true;
        }
        return false;
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
                
                // 检查任务是否仍然有效
                auto it = active_tasks_.find(next_task.id);
                if (it != active_tasks_.end() && !it->second->completed) {
                    active_tasks_.erase(it);
                    
                    // 执行回调（非阻塞定时器）
                    if (next_task.mode == NON_BLOCKING) {
                        std::cout << "非阻塞定时器触发: ID=" << next_task.id << std::endl;
                        // 在新线程中执行回调，避免阻塞管理器线程
                        std::thread callback_thread([callback = next_task.callback]() {
                            callback();
                        });
                        callback_thread.detach();
                    }
                }
            } else {
                // 等待直到下一个定时器到期
                cv_.wait_until(lock, wake_time);
            }
        }
    }
};
std::atomic<bool> slpmod_flag{false};
bool slpmod_prests = false;
std::atomic_bool slpmod_nonoff2off_auto_run{false};
int timer1;

// 示例回调函数
void timerCallback(int timer_id) {
    std::cout << "回调函数执行: 定时器" << timer_id << "时间到!" << std::endl;
}

void complexTask() {
    std::cout << "执行复杂任务..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "复杂任务完成" << std::endl;
}

// void tfunc(){
//     while(slpmod_nonoff2off_auto_run){
//         timer1 = timer.addNonBlockingTimer(std::chrono::seconds(5), [&](){
//             if (slpmod_prests == true) {
//                 timer.cancelTimer(timer1);
//                 std::cout << "Timer canceled" << std::endl;
//             } else {
//                 slpmod_nonoff2off_auto_run = false;
//             }
//             std::cout << "while quit!" << std::endl;
//         });
//     }
// }
int ktime = 1;

std::atomic_bool slp_stop{false};

void pwrcheck(){
    while(!slp_stop){
        std::chrono::steady_clock::time_point next_run{std::chrono::steady_clock::now()};
        std::chrono::steady_clock::duration duration{std::chrono::seconds(1)};
        
        std::cout << "slp_stop: " << slp_stop << std::endl;

        next_run += duration;
        std::this_thread::sleep_until(next_run);
        
    } // 电源状态判断，置不同flag
        std::cout << "outside slp_stop: " << slp_stop << std::endl;

}

void test(){
    std::chrono::steady_clock::time_point start_time{std::chrono::steady_clock::now()};
    std::chrono::steady_clock::duration duration{std::chrono::milliseconds(100)};
    int count = (ktime * 10 * 1000) / 100;
    while(count){
        
        std::cout << "COunt = " << count << "duration: " << duration.count() << " ms" << std::endl;
        if(slp_stop){
            count = (ktime * 1 * 1000) / 100;
        } else{
            count--;
        }
        start_time += duration;
        std::this_thread::sleep_until(start_time);
    }
}

// func 逻辑判断

int main() {

    int time_eslap = 10;

    DualModeTimer timer;
    timer.start();
    
    std::cout << "=== 阻塞定时器示例 ===" << std::endl;

    auto timer_2 = timer.addNonBlockingTimer(std::chrono::seconds(10), [&time_eslap](){
        // std::chrono::steady_clock::time_point next_run{std::chrono::steady_clock::now()};
        // std::chrono::steady_clock::duration duration{std::chrono::seconds(1)};
        // while(time_eslap){
        //     time_eslap--;
        //     std::cout << "time_eslap = " << time_eslap << std::endl;
        //     next_run += duration;
        //     std::this_thread::sleep_until(next_run);
        //     // std::this_thread::sleep_for(std::chrono::seconds(1));
        // }
        slp_stop = true;
    });

    std::thread tt(pwrcheck);
    std::thread test1(test);
    
    // std::thread ([&](){
    //     if(time_eslap < 5) {
    //      timer.cancelTimer(timer_2);
    //      time_eslap = 10;

    //     }
    // });
    // auto strttime = std::chrono::steady_clock::now();
    // timer.addBlockingTimer(std::chrono::seconds(5));

    // std::thread slptrans([&](){while(!slpmod_flag){
    //     std::this_thread::sleep_for(std::chrono::seconds(10));
    //     slpmod_nonoff2off_auto_run = true;
    // }});

    // std::thread check([&](){
    //     std::this_thread::sleep_for(std::chrono::seconds(3));
    //     slpmod_prests = true;
    // });

    // auto endtime = std::chrono::steady_clock::now();
    // auto time_duration = endtime - strttime;
    // std::cout << "时间段：" << std::chrono::duration_cast<std::chrono::seconds>(time_duration).count() << std::endl;

    // 在主线程中使用阻塞定时器
    // std::thread main_thread([&timer]() {
    //     std::cout << "主线程开始工作" << std::endl;
    //     auto start = std::chrono::steady_clock::now();
        // 阻塞定时器：会阻塞当前线程3秒
        // timer.addBlockingTimer(std::chrono::seconds(3));
        // auto time_end = std::chrono::steady_clock::now();

        // auto time_duration = time_end - start;
        // std::cout << "时间段：" << std::chrono::duration_cast<std::chrono::seconds>(time_duration).count() << std::endl;
        
        // std::cout << "主线程继续工作" << std::endl;
        
        // // 再阻塞2秒
        // timer.addBlockingTimer(std::chrono::seconds(2));
        
        // std::cout << "主线程工作完成" << std::endl;
    // });
    
    // std::cout << "=== 非阻塞定时器示例 ===" << std::endl;
    
    // // 添加多个非阻塞定时器
    // int timer1 = timer.addNonBlockingTimer(std::chrono::seconds(2), 
    //                                      []() { timerCallback(1); });
    
    // int timer2 = timer.addNonBlockingTimer(std::chrono::seconds(4), 
    //                                      []() { complexTask(); });
    
    // int timer3 = timer.addNonBlockingTimer(std::chrono::seconds(1), 
    //                                      []() { timerCallback(3); });
    
    // // 主线程可以继续执行其他任务
    // std::cout << "主线程执行其他任务..." << std::endl;
    // for (int i = 0; i < 5; ++i) {
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    //     std::cout << "主任务进度: " << (i + 1) << "/5" << std::endl;
    // }
    
    // // 可以取消尚未执行的定时器
    // timer.cancelTimer(timer2);
    // std::cout << "定时器2已取消" << std::endl;
    
    // main_thread.join();
    tt.join();
    test1.join();
    
    // 等待所有非阻塞定时器完成
    std::this_thread::sleep_for(std::chrono::seconds(1));
    timer.stop();
    
    return 0;
}