#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <atomic>
#include <condition_variable>

class A {
private:
    int shared_param_;          
    mutable std::mutex mtx_;
    std::atomic<bool> running_;  // 使用原子变量控制线程状态
    std::condition_variable cv_; // 用于线程间同步

public:
    A() : shared_param_(0), running_(false) {
        std::srand(std::time(nullptr));
    }

    ~A() {
        stop(); // 析构时自动停止
    }

    void start() {
        if (running_.exchange(true)) {
            return; // 已经在运行
        }
        
        // 启动生成参数的线程
        std::thread(&A::generateParam, this).detach();
    }

    void stop() {
        running_ = false;
        cv_.notify_all(); // 唤醒可能正在等待的线程
    }

    void generateParam() {
        while (running_.load()) {
            int param = std::rand() % 100;

            {
                std::lock_guard<std::mutex> lock(mtx_);
                shared_param_ = param;
                std::cout << "A生成参数：" << param << std::endl;
            }
            cv_.notify_all(); // 通知所有等待的线程数据已更新

            // 使用条件变量等待，支持被提前唤醒停止
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait_for(lock, std::chrono::seconds(1), 
                        [this]() { return !running_.load(); });
        }
    }

    int getSharedParam() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return shared_param_;
    }

    // 新增：等待参数变化的接口
    bool waitForParamChange(int& param, std::chrono::milliseconds timeout = std::chrono::milliseconds(500)) {
        std::unique_lock<std::mutex> lock(mtx_);
        int current_param = shared_param_;
        
        if (cv_.wait_for(lock, timeout, [this, current_param]() {
            return shared_param_ != current_param || !running_.load();
        })) {
            if (running_.load()) {
                param = shared_param_;
                return true;
            }
        }
        return false;
    }
};

class B {
private:
    A* a_ptr_;
    std::atomic<bool> running_;

public:
    B(A* a) : a_ptr_(a), running_(false) {}

    ~B() {
        stop();
    }

    void start() {
        if (running_.exchange(true)) {
            return;
        }
        
        // 启动使用参数的线程
        std::thread(&B::useParam, this).detach();
    }

    void stop() {
        running_ = false;
    }

    void useParam() {
        int last_param = -1;
        
        while (running_.load()) {
            int current_param = a_ptr_->getSharedParam();
            
            // 只有当参数变化时才打印，减少输出噪音
            if (current_param != last_param) {
                std::cout << "B获取参数：" << current_param << std::endl;
                last_param = current_param;
            }

            // 使用条件变量等待，支持优雅停止
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    // 新增：使用事件驱动的方式监听参数变化
    void useParamEventDriven() {
        while (running_.load()) {
            int new_param;
            if (a_ptr_->waitForParamChange(new_param)) {
                std::cout << "B事件驱动获取新参数：" << new_param << std::endl;
                processParam(new_param);
            }
        }
    }

private:
    void processParam(int param) {
        // 模拟参数处理逻辑
        if (param > 50) {
            std::cout << "B处理：参数值较大(" << param << ")" << std::endl;
        } else {
            std::cout << "B处理：参数值较小(" << param << ")" << std::endl;
        }
    }
};

int main() {
    std::cout << "=== 优化版A/B类测试 ===" << std::endl;

    A a;
    B b(&a);

    // 启动A和B
    a.start();
    b.start();

    // 让程序运行一段时间
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "准备停止程序..." << std::endl;

    // 优雅停止
    b.stop();
    a.stop();

    // 等待一段时间确保线程停止
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "程序结束" << std::endl;
    return 0;
}