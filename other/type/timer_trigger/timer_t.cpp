#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <condition_variable>

class SoftwareTimer{
public:
    SoftwareTimer(std::chrono::milliseconds interval, int max_count)
        : interval_(interval), max_count_(max_count), count_(0), running_(false) {}
    
    void start(std::function<void(int)>callback){
        if (running_.exchange(true)) return;

        count_ = 0;

        // 创建定时器线程
        timer_thread_ = std::thread([this, callback]() {
            auto next_time = std::chrono::steady_clock::now() + interval_;

            for (int i = 0; i < max_count_ && running_; i++) {
                std::this_thread::sleep_until(next_time);

                if (!running_) break;

                // 调用回调函数
                callback(++count_);
                // 更新下次触发时间
                next_time += interval_;
            }

            running_ = false;
            cv_.notify_one(); // 通知等待线程
        });
        
    }

    void stop() {
        if (running_.exchange(false)) {
            if (timer_thread_.joinable()) {
                timer_thread_.join(); // 等待线程结束
            }
        }
    }

    bool is_running() const {
        return running_;
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] {return !running_;});
    }

    ~SoftwareTimer(){
        stop(); // 确保在销毁时停止定时器
    }


private:
    std::chrono::milliseconds interval_;
    int max_count_;
    std::atomic<int> count_;
    std::atomic<bool> running_;
    std::thread timer_thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

int main() {
    constexpr int max_count = 10;
    constexpr std::chrono::milliseconds interval(1000);

    SoftwareTimer timer(interval, max_count);

    std::cout << "Timer started. Output 'Hello World' 10 times...\n";

    timer.start([](int count){
        std::cout << "Hello World! (" << count << "/" << max_count << ")\n";
    });

    // 等待定时器完成
    timer.wait();
    std::cout << "Timer Completed. Exiting...\n";
    return 0;
}