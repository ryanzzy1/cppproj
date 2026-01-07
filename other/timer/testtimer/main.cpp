// main.cpp
#include "Timer.hpp"
// #include "TimerManager.hpp"
#include <iostream>
#include <iomanip>

void exampleUsage() {
    std::cout << "=== 定时器使用示例 ===\n";
    
    // 创建定时器
    Timer timer;
    
    // 示例1: 阻塞定时
    std::cout << "\n1. 阻塞定时示例:\n";
    std::cout << "   开始阻塞3秒...\n";
    
    std::thread cancelThread([&timer]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "   尝试取消阻塞定时...\n";
        // 注意：这里无法取消，因为blockFor没有返回ID
        // 如果需要取消，应该使用asyncAfter
    });
    
    bool success = timer.blockFor(std::chrono::seconds(3));
    std::cout << "   阻塞定时结果: " << (success ? "成功" : "被取消") << "\n";
    
    if (cancelThread.joinable()) cancelThread.join();
    
    // 示例2: 异步定时（可取消）
    std::cout << "\n2. 异步定时示例:\n";
    auto future1 = timer.asyncAfter(std::chrono::seconds(2), []() {
        std::cout << "   定时器回调执行！\n";
    });
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 获取定时器ID（在实际使用中，需要修改接口以返回ID）
    // 这里假设我们有一个方法获取最后一个定时器ID
    
    // 示例3: 周期定时
    std::cout << "\n3. 周期定时示例:\n";
    Timer::TimerId periodicId = timer.asyncEvery(std::chrono::seconds(1), [count = 0]() mutable {
        std::cout << "   周期定时 " << ++count << "\n";
        if (count >= 5) {
            // 注意：这里无法停止，需要外部取消
        }
    });
    
    std::this_thread::sleep_for(std::chrono::seconds(6));
    timer.cancel(periodicId);
    std::cout << "   周期定时已取消\n";
    /*
    // 示例4: 使用TimerManager
    std::cout << "\n4. TimerManager示例:\n";
    TimerManager manager;
    manager.start();
    
    // 添加单次任务
    manager.scheduleOneShot(std::chrono::seconds(2), []() {
        std::cout << "   单次任务执行\n";
    });
    
    // 添加周期任务
    auto periodicTaskId = manager.schedulePeriodic(std::chrono::seconds(1), []() {
        static int count = 0;
        std::cout << "   周期任务 " << ++count << "\n";
    });
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    // 取消周期任务
    manager.cancel(periodicTaskId);
    std::cout << "   周期任务已取消\n";
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    manager.stop();*/
    
    std::cout << "\n=== 示例完成 ===\n";
}

void performanceTest() {
    std::cout << "\n=== 性能测试 ===\n";
    
    Timer timer;
    constexpr int NUM_TIMERS = 1000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::future<bool>> futures;
    for (int i = 0; i < NUM_TIMERS; ++i) {
        futures.push_back(timer.asyncAfter(
            std::chrono::milliseconds(10 + i % 100),
            [i]() { /* 空回调 */ }
        ));
    }
    
    // 随机取消一些定时器
    for (int i = 0; i < NUM_TIMERS / 10; ++i) {
        // 注意：这里需要知道timer ID，实际使用时需要修改接口
    }
    
    // 等待所有定时器完成
    for (auto& future : futures) {
        future.wait();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "创建并等待 " << NUM_TIMERS << " 个定时器耗时: " 
              << duration.count() << "ms\n";
}

int main() {
    try {
        exampleUsage();
        performanceTest();
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}