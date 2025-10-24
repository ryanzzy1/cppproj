#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>

class Timer {
public:
    Timer() : active_(false) {}
    
    ~Timer() {
        stop();
    }
    
    // 设置定时器（单次执行）
    template<typename Function>
    void setTimeout(Function function, int delay_ms) {
        stop();
        active_ = true;
        thread_ = std::thread([=]() {
            std::unique_lock<std::mutex> lock(mutex_);
            if (cv_.wait_for(lock, std::chrono::milliseconds(delay_ms), [this]() { 
                return !active_.load(); 
            })) {
                // 定时器被取消
                return;
            }
            if (active_.load()) {
                function();
            }
        });
    }
    
    // 设置定时器（循环执行）
    template<typename Function>
    void setInterval(Function function, int interval_ms) {
        stop();
        active_ = true;
        thread_ = std::thread([=]() {
            while (active_.load()) {
                std::unique_lock<std::mutex> lock(mutex_);
                if (cv_.wait_for(lock, std::chrono::milliseconds(interval_ms), [this]() { 
                    return !active_.load(); 
                })) {
                    // 定时器被取消
                    break;
                }
                if (active_.load()) {
                    function();
                }
            }
        });
    }
    
    // 停止定时器
    void stop() {
        active_ = false;
        cv_.notify_all();
        if (thread_.joinable()) {
            thread_.join();
        }
    }
    
    // 重置定时器
    void reset() {
        stop();
    }
    
private:
    std::atomic<bool> active_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
};


// 测试代码
int main() {
    std::cout << "=== 基础定时器测试 ===" << std::endl;
    
    Timer timer;
    
    // 单次定时器
    std::cout << "设置3秒后执行的单次定时器..." << std::endl;
    timer.setTimeout([]() {
        std::cout << "单次定时器触发！时间: " 
                  << std::chrono::system_clock::now().time_since_epoch().count() 
                  << std::endl;
    }, 3000);
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "2秒后重置定时器..." << std::endl;
    timer.reset();
    
    // 循环定时器
    std::cout << "\n设置每1秒执行的循环定时器..." << std::endl;
    int count = 0;
    timer.setInterval([&count]() {
        std::cout << "循环定时器触发，次数: " << ++count 
                  << "，时间: " << std::chrono::system_clock::now().time_since_epoch().count() 
                  << std::endl;
        
        if (count >= 5) {
            std::cout << "达到5次，程序结束" << std::endl;
            exit(0);
        }
    }, 1000);
    
    // 让程序运行足够长时间来观察定时器行为
    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    return 0;
}