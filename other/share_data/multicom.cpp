#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <queue>
#include <memory>

// 线程安全的消息队列（支持停止机制）
template<typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex mtx_;
    std::queue<T> queue_;
    std::condition_variable cv_;
    std::atomic<bool> stop_flag_{false};

public:
    void push(T value) {
        if (stop_flag_.load()) return;
        
        std::lock_guard<std::mutex> lock(mtx_);
        queue_.push(std::move(value));
        cv_.notify_one();
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (queue_.empty() || stop_flag_.load()) {
            return false;
        }
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    bool wait_and_pop(T& value, std::chrono::milliseconds timeout = std::chrono::milliseconds(100)) {
        std::unique_lock<std::mutex> lock(mtx_);
        
        // 使用带超时的等待，避免永久阻塞
        if (!cv_.wait_for(lock, timeout, [this]() { 
            return !queue_.empty() || stop_flag_.load(); 
        })) {
            return false; // 超时
        }
        
        if (stop_flag_.load() || queue_.empty()) {
            return false;
        }
        
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void stop() {
        stop_flag_.store(true);
        cv_.notify_all(); // 唤醒所有等待的线程
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.empty();
    }
    
    bool is_stopped() const {
        return stop_flag_.load();
    }
};

class AdvancedA {
private:
    int shared_param_;
    mutable std::mutex mtx_;
    std::atomic<bool> running_;
    std::condition_variable cv_;
    
    ThreadSafeQueue<int> param_queue_;
    std::function<void(int)> callback_;
    
    std::thread worker_thread_; // 保存线程对象

public:
    AdvancedA() : shared_param_(0), running_(false) {
        std::srand(std::time(nullptr));
    }

    ~AdvancedA() {
        stop();
    }

    void start() {
        if (running_.exchange(true)) return;
        
        // 使用成员变量保存线程，避免detach
        worker_thread_ = std::thread(&AdvancedA::generateParam, this);
    }

    void stop() {
        if (!running_.exchange(false)) return;
        
        cv_.notify_all();
        param_queue_.stop();
        
        // 等待线程结束
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
        
        std::cout << "AdvancedA 已停止" << std::endl;
    }

    void setCallback(std::function<void(int)> callback) {
        std::lock_guard<std::mutex> lock(mtx_);
        callback_ = std::move(callback);
    }

    void generateParam() {
        std::cout << "AdvancedA 线程开始" << std::endl;
        
        while (running_.load()) {
            int param = std::rand() % 100;

            {
                std::lock_guard<std::mutex> lock(mtx_);
                shared_param_ = param;
                if (!param_queue_.is_stopped()) {
                    param_queue_.push(param);
                }
                
                std::cout << "AdvancedA生成参数：" << param << std::endl;
                
                if (callback_) {
                    callback_(param);
                }
            }
            
            cv_.notify_all();

            // 使用可中断的等待
            std::unique_lock<std::mutex> lock(mtx_);
            if (cv_.wait_for(lock, std::chrono::seconds(1), 
                            [this]() { return !running_.load(); })) {
                break; // 被停止信号唤醒
            }
            // 否则超时后继续循环
        }
        
        std::cout << "AdvancedA 线程结束" << std::endl;
    }

    int getSharedParam() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return shared_param_;
    }

    bool getLatestParam(int& param) {
        return param_queue_.try_pop(param);
    }

    bool waitForParam(int& param, std::chrono::milliseconds timeout = std::chrono::milliseconds(100)) {
        return param_queue_.wait_and_pop(param, timeout);
    }
    
    bool isRunning() const {
        return running_.load();
    }
};

class AdvancedB {
private:
    std::shared_ptr<AdvancedA> a_ptr_; // 使用shared_ptr管理生命周期
    std::atomic<bool> running_;
    
    std::thread polling_thread_;
    std::thread event_thread_;

public:
    AdvancedB(std::shared_ptr<AdvancedA> a) : a_ptr_(a), running_(false) {}

    ~AdvancedB() {
        stop();
    }

    void start() {
        if (running_.exchange(true)) return;
        
        // 启动轮询模式线程
        polling_thread_ = std::thread(&AdvancedB::pollingMode, this);
        
        // 启动事件驱动模式线程
        event_thread_ = std::thread(&AdvancedB::eventDrivenMode, this);
    }

    void stop() {
        if (!running_.exchange(false)) return;
        
        // 等待线程结束
        if (polling_thread_.joinable()) {
            polling_thread_.join();
        }
        if (event_thread_.joinable()) {
            event_thread_.join();
        }
        
        std::cout << "AdvancedB 已停止" << std::endl;
    }

private:
    // 轮询模式
    void pollingMode() {
        std::cout << "B轮询模式线程开始" << std::endl;
        
        int last_param = -1;
        
        while (running_.load() && a_ptr_ && a_ptr_->isRunning()) {
            int current_param = a_ptr_->getSharedParam();
            
            if (current_param != last_param) {
                std::cout << "B轮询模式获取参数：" << current_param << std::endl;
                last_param = current_param;
            }

            // 使用可中断的睡眠
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
        
        std::cout << "B轮询模式线程结束" << std::endl;
    }

    // 事件驱动模式
    void eventDrivenMode() {
        std::cout << "B事件驱动模式线程开始" << std::endl;
        
        while (running_.load() && a_ptr_ && a_ptr_->isRunning()) {
            int param;
            if (a_ptr_->waitForParam(param, std::chrono::milliseconds(500))) {
                std::cout << "B事件驱动模式获取参数：" << param << std::endl;
                processParam(param);
            }
            // 超时后检查是否继续运行
        }
        
        std::cout << "B事件驱动模式线程结束" << std::endl;
    }

    void processParam(int param) {
        // 更复杂的处理逻辑
        if (param > 80) {
            std::cout << "  警告：参数值过高(" << param << ")" << std::endl;
        } else if (param < 20) {
            std::cout << "  提示：参数值过低(" << param << ")" << std::endl;
        } else {
            std::cout << "  正常：参数值适中(" << param << ")" << std::endl;
        }
    }
};

// 回调函数示例
void externalProcessor(int param) {
    static int count = 0;
    std::cout << "外部处理器接收参数[" << ++count << "]：" << param << std::endl;
}

int main() {
    std::cout << "=== 修复版A/B类测试 ===" << std::endl;

    // 使用shared_ptr管理A的生命周期
    auto a = std::make_shared<AdvancedA>();
    AdvancedB b(a);

    // 设置回调函数
    a->setCallback(externalProcessor);

    // 启动
    a->start();
    b.start();

    // 运行10秒
    std::cout << "程序运行中..." << std::endl;
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "运行时间: " << (i + 1) << "秒" << std::endl;
    }

    std::cout << "准备停止程序..." << std::endl;

    // 按正确顺序停止：先停止B，再停止A
    b.stop();
    a->stop();

    std::cout << "所有线程已停止，程序正常退出" << std::endl;
    return 0;
}