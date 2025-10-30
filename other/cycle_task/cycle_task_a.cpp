#include <iostream>
#include <chrono>
#include <functional>
#include <vector>
#include <unordered_map>
#include <thread>
#include <atomic>

class PeriodicScheduler {
private:
    struct Task {
        std::function<void()> function;
        std::chrono::milliseconds interval;
        std::chrono::steady_clock::time_point nextExecution;
    };
    
    std::vector<Task> tasks;
    std::atomic<bool> running{false};
    
public:
    // 添加周期性任务
    void addTask(std::function<void()> func, std::chrono::milliseconds interval) {
        Task task{
            func,
            interval,
            std::chrono::steady_clock::now() + interval
        };
        tasks.push_back(task);
    }
    
    // 启动调度器
    void start() {
        running = true;
        while (running) {
            auto now = std::chrono::steady_clock::now();
            
            for (auto& task : tasks) {
                if (now >= task.nextExecution) {
                    task.function();  // 执行任务
                    task.nextExecution = now + task.interval;
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    void stop() {
        running = false;
    }
};

// 使用示例
void task1() { std::cout << "Task1 - 100ms周期\n"; }
void task2() { std::cout << "Task2 - 500ms周期\n"; }
void task3() { std::cout << "Task3 - 1000ms周期\n"; }

int main() {
    PeriodicScheduler scheduler;
    
    scheduler.addTask(task1, std::chrono::milliseconds(100));
    scheduler.addTask(task2, std::chrono::milliseconds(500));
    scheduler.addTask(task3, std::chrono::milliseconds(1000));
    
    std::thread schedulerThread([&](){ scheduler.start(); });
    
    // 运行10秒后停止
    std::this_thread::sleep_for(std::chrono::seconds(10));
    scheduler.stop();
    schedulerThread.join();
    
    return 0;
}