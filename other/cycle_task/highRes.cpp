#include <chrono>
#include <vector>
#include <algorithm>
#include <functional>
#include <thread>

class HighPrecisionScheduler {
private:
    struct HighPrecisionTask {
        std::function<void()> callback;
        std::chrono::microseconds interval;
        std::chrono::high_resolution_clock::time_point nextRun;
        uint64_t executionCount = 0;
    };
    
    std::vector<HighPrecisionTask> tasks;
    std::chrono::microseconds minInterval{1000}; // 1ms最小间隔
    
public:
    void addHighPrecisionTask(std::function<void()> func, 
                             std::chrono::milliseconds interval) {
        HighPrecisionTask task{
            func,
            std::chrono::duration_cast<std::chrono::microseconds>(interval),
            std::chrono::high_resolution_clock::now() + 
                std::chrono::duration_cast<std::chrono::microseconds>(interval)
        };
        tasks.push_back(task);
        
        // 更新最小间隔
        if (task.interval < minInterval) {
            minInterval = task.interval;
        }
    }
    
    void run() {
        while (true) {
            auto now = std::chrono::high_resolution_clock::now();
            
            for (auto& task : tasks) {
                if (now >= task.nextRun) {
                    auto start = std::chrono::high_resolution_clock::now();
                    
                    task.callback();  // 执行任务
                    task.executionCount++;
                    
                    auto end = std::chrono::high_resolution_clock::now();
                    auto executionTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    
                    // 计算下一次执行时间，考虑执行耗时
                    task.nextRun = now + task.interval - executionTime;
                }
            }
            
            // 高精度休眠
            std::this_thread::sleep_for(minInterval / 2);
        }
    }
};