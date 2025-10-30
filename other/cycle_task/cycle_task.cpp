#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <thread>

// 任务结构体：存储函数、周期和上次执行时间
struct PeriodicTask {
    std::function<void()> func;  // 要执行的函数
    std::chrono::milliseconds period;  // 执行周期（毫秒）
    std::chrono::steady_clock::time_point last_run;  // 上次执行时间
};

// 单线程调度器
class SingleThreadScheduler {
private:
    std::vector<PeriodicTask> tasks;  // 任务列表
    bool running = false;  // 调度器运行标志

public:
    // 添加任务：传入函数和周期（毫秒）
    void addTask(std::function<void()> func, int period_ms) {
        tasks.push_back({
            func,
            std::chrono::milliseconds(period_ms),
            std::chrono::steady_clock::now()  // 初始时间设为当前
        });
    }

    // 启动调度器（单线程循环）
    void start() {
        running = true;
        while (running) {
            auto now = std::chrono::steady_clock::now();  // 当前时间

            // 遍历所有任务，检查是否需要执行
            for (auto& task : tasks) {
                // 计算距离上次执行的时间差
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - task.last_run
                );

                // 若时间差 >= 周期，则执行函数并更新上次执行时间
                if (elapsed >= task.period) {
                    task.func();  // 执行任务
                    task.last_run = now;  // 更新时间戳（也可累加周期，避免漂移）
                }
            }

            // 短暂休眠，减少CPU占用（休眠时间可设为最小周期的1/10，平衡精度和性能）
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    // 停止调度器
    void stop() {
        running = false;
    }
};

// ------------------------------
// 测试：定义3个不同周期的函数
// ------------------------------
void func1() {  // 100ms周期
    static int count = 0;
    std::cout << "func1（100ms）执行次数：" << ++count << std::endl;
}

void func2() {  // 200ms周期
    static int count = 0;
    std::cout << "func2（200ms）执行次数：" << ++count << std::endl;
}

void func3() {  // 500ms周期
    static int count = 0;
    std::cout << "func3（500ms）执行次数：" << ++count << std::endl;
}

int main() {
    SingleThreadScheduler scheduler;

    // 添加任务（函数+周期）
    scheduler.addTask(func1, 100);  // 每100ms执行一次
    scheduler.addTask(func2, 200);  // 每200ms执行一次
    scheduler.addTask(func3, 500);  // 每500ms执行一次

    // 启动调度器（单线程阻塞运行）
    std::cout << "启动调度器（3秒后停止）..." << std::endl;
    std::thread t(&SingleThreadScheduler::start, &scheduler);

    // 运行3秒后停止
    std::this_thread::sleep_for(std::chrono::seconds(3));
    scheduler.stop();
    t.join();

    return 0;
}
