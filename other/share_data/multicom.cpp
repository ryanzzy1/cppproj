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

// 线程安全的消息队列
template<typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex mtx_;
    std::queue<T> queue_;
    std::condition_variable cv_;
    std::atomic<bool> stop_{false};

public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(mtx_);
        if(stop_) return;
        queue_.push(std::move(value));
        cv_.notify_one();
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (queue_.empty() || stop_) {
            return false;
        }

        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    bool wait_and_pop(T& value){
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this](){return stop_ || !queue_.empty();});
        if(stop_ && queue_.empty()) {
            return false;
        }

        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void stop() {
        stop_.store(true);
        cv_.notify_all();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.empty();
    }
};

class AdvancedA {
private:
    int shared_param_;
    mutable std::mutex mtx_;
    std::atomic<bool> running_;
    std::condition_variable cv_;
    
    // 支持多种通信方式
    ThreadSafeQueue<int> param_queue_;
    std::function<void(int)> callback_;

public:
    AdvancedA() : shared_param_(0), running_(false) {
        std::srand(std::time(nullptr));
    }

    ~AdvancedA() {
        stop();
    }

    void start() {
        if (running_.exchange(true)) return;
        std::thread(&AdvancedA::generateParam, this).detach();
    }

    void stop() {
        running_ = false;
        param_queue_.stop();
        cv_.notify_all();
    }

    // 设置回调函数
    void setCallback(std::function<void(int)> callback) {
        std::lock_guard<std::mutex> lock(mtx_);
        callback_ = std::move(callback);
    }

    void generateParam() {
        while (running_.load()) {
            int param = std::rand() % 100;

            {
                std::lock_guard<std::mutex> lock(mtx_);
                shared_param_ = param;
                param_queue_.push(param);
                
                std::cout << "AdvancedA生成参数：" << param << std::endl;
                
                // 如果有回调函数，调用它
                if (callback_) {
                    callback_(param);
                }
            }
            
            cv_.notify_all();

            // 等待1秒或直到被停止
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait_for(lock, std::chrono::seconds(1), 
                        [this]() { return !running_.load(); });
        }
    }

    int getSharedParam() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return shared_param_;
    }

    bool getLatestParam(int& param) {
        return param_queue_.try_pop(param);
    }

    bool waitForParam(int& param) {
       return param_queue_.wait_and_pop(param);
    }
};

class AdvancedB {
private:
    AdvancedA* a_ptr_;
    std::atomic<bool> running_;

public:
    AdvancedB(AdvancedA* a) : a_ptr_(a), running_(false) {}

    ~AdvancedB() {
        stop();
    }

    void start() {
        if (running_.exchange(true)) return;
        
        // 可以启动多种监听模式
        std::thread(&AdvancedB::pollingMode, this).detach();
        std::thread(&AdvancedB::eventDrivenMode, this).detach();
    }

    void stop() {
        running_ = false;
    }

private:
    // 轮询模式
    void pollingMode() {
        int last_param = -1;
        
        while (running_.load()) {
            int current_param = a_ptr_->getSharedParam();
            
            if (current_param != last_param) {
                std::cout << "B轮询模式获取参数：" << current_param << std::endl;
                last_param = current_param;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }

    // 事件驱动模式
    void eventDrivenMode() {
        while (running_.load()) {
            int param;
            if (!a_ptr_->waitForParam(param)) {
                break;
            }

            std::cout << "B事件驱动模式获取参数：" << param << std::endl;
            processParam(param);
        }
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
    std::cout << "=== 高级版A/B类测试 ===" << std::endl;

    AdvancedA a;
    AdvancedB b(&a);

    // 设置回调函数
    a.setCallback(externalProcessor);

    // 启动
    a.start();
    b.start();

    // 运行15秒
    std::this_thread::sleep_for(std::chrono::seconds(15));

    std::cout << "准备停止程序..." << std::endl;

    // 优雅停止
    b.stop();
    a.stop();

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "程序结束" << std::endl;

    return 0;
}