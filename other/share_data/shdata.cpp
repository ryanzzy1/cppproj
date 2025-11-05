
// 方案1： 接口设计模式
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>

// 前置声明
class C;

class C {
private:
    int data_;
    mutable std::mutex mtx_;
    std::string name_;

public:
    C(const std::string& name = "C") : data_(0), name_(name) {}
    
    void updateData(int new_data) {
        std::lock_guard<std::mutex> lock(mtx_);
        data_ = new_data;
        std::cout << name_ << " 数据更新为: " << data_ << std::endl;
    }
    
    int getData() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return data_;
    }
    
    // 线程安全的复杂操作
    void complexOperation(int value) {
        std::lock_guard<std::mutex> lock(mtx_);
        data_ += value;
        std::cout << name_ << " 执行复杂操作，数据变为: " << data_ << std::endl;
    }
    
    ~C() {
        std::cout << name_ << " 被销毁" << std::endl;
    }
};

class A {
private:
    std::unique_ptr<C> c_ptr_;
    mutable std::mutex mtx_;
    std::atomic<bool> running_{false};
    std::thread worker_thread_;

public:
    A() : c_ptr_(std::make_unique<C>("C_from_A")) {}
    
    ~A() {
        stop();
    }
    
    void start() {
        if (running_.exchange(true)) return;
        
        worker_thread_ = std::thread(&A::workerThread, this);
        std::cout << "A 开始运行" << std::endl;
    }
    
    void stop() {
        if (!running_.exchange(false)) return;
        
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
        std::cout << "A 停止运行" << std::endl;
    }
    
    // 为B类提供的安全接口
    int getCData() const {
        std::lock_guard<std::mutex> lock(mtx_);
        if (c_ptr_) {
            return c_ptr_->getData();
        }
        return -1; // 或者抛出异常
    }
    
    // 执行C类的复杂操作（通过A类代理）
    void executeCComplexOperation(int value) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (c_ptr_) {
            c_ptr_->complexOperation(value);
        }
    }
    
    // 检查C是否可用
    bool isCAvailable() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return c_ptr_ != nullptr;
    }

private:
    void workerThread() {
        int counter = 0;
        while (running_.load()) {
            // 更新C的数据
            {
                std::lock_guard<std::mutex> lock(mtx_);
                if (c_ptr_) {
                    c_ptr_->updateData(++counter);
                }
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
};

class B {
private:
    std::shared_ptr<A> a_ptr_;
    std::atomic<bool> running_{false};
    std::thread worker_thread_;

public:
    B(std::shared_ptr<A> a) : a_ptr_(a) {}
    
    ~B() {
        stop();
    }
    
    void start() {
        if (running_.exchange(true)) return;
        
        worker_thread_ = std::thread(&B::workerThread, this);
        std::cout << "B 开始运行" << std::endl;
    }
    
    void stop() {
        if (!running_.exchange(false)) return;
        
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
        std::cout << "B 停止运行" << std::endl;
    }

private:
    void workerThread() {
        while (running_.load()) {
            if (a_ptr_ && a_ptr_->isCAvailable()) {
                // 通过A类的接口安全地访问C的数据
                int c_data = a_ptr_->getCData();
                std::cout << "B 获取到C的数据: " << c_data << std::endl;
                
                // 通过A类执行C的复杂操作
                if (c_data % 5 == 0) { // 每5次执行一次复杂操作
                    a_ptr_->executeCComplexOperation(10);
                }
            } else {
                std::cout << "B: A或C不可用" << std::endl;
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};


/*
// 方案2：观察者模式
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <vector>

class C {
private:
    int data_;
    mutable std::mutex mtx_;
    std::vector<std::function<void(int)>> observers_;

public:
    C() : data_(0) {}
    
    void updateData(int new_data) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            data_ = new_data;
        }
        notifyObservers(new_data);
    }
    
    int getData() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return data_;
    }
    
    void addObserver(std::function<void(int)> observer) {
        std::lock_guard<std::mutex> lock(mtx_);
        observers_.push_back(observer);
    }
    
    void removeAllObservers() {
        std::lock_guard<std::mutex> lock(mtx_);
        observers_.clear();
    }

private:
    void notifyObservers(int data) {
        std::vector<std::function<void(int)>> observers_copy;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            observers_copy = observers_;
        }
        
        for (const auto& observer : observers_copy) {
            observer(data);
        }
    }
};

class A {
private:
    std::unique_ptr<C> c_ptr_;
    std::atomic<bool> running_{false};
    std::thread worker_thread_;

public:
    A() : c_ptr_(std::make_unique<C>()) {}
    
    ~A() {
        stop();
        if (c_ptr_) {
            c_ptr_->removeAllObservers();
        }
    }
    
    void start() {
        if (running_.exchange(true)) return;
        
        worker_thread_ = std::thread(&A::workerThread, this);
        std::cout << "A 开始运行" << std::endl;
    }
    
    void stop() {
        if (!running_.exchange(false)) return;
        
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
        std::cout << "A 停止运行" << std::endl;
    }
    
    // 允许B类注册观察者
    void registerCObserver(std::function<void(int)> observer) {
        if (c_ptr_) {
            c_ptr_->addObserver(observer);
        }
    }
    
    // 获取C的当前数据
    int getCData() const {
        if (c_ptr_) {
            return c_ptr_->getData();
        }
        return -1;
    }

private:
    void workerThread() {
        int counter = 0;
        while (running_.load()) {
            if (c_ptr_) {
                c_ptr_->updateData(++counter);
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
};

class B {
private:
    std::shared_ptr<A> a_ptr_;
    std::atomic<bool> running_{false};
    std::thread worker_thread_;
    int last_c_data_ = -1;

public:
    B(std::shared_ptr<A> a) : a_ptr_(a) {
        // 注册为C的观察者
        if (a_ptr_) {
            a_ptr_->registerCObserver([this](int data) {
                this->onCDataChanged(data);
            });
        }
    }
    
    ~B() {
        stop();
    }
    
    void start() {
        if (running_.exchange(true)) return;
        
        worker_thread_ = std::thread(&B::workerThread, this);
        std::cout << "B 开始运行" << std::endl;
    }
    
    void stop() {
        if (!running_.exchange(false)) return;
        
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
        std::cout << "B 停止运行" << std::endl;
    }

private:
    void onCDataChanged(int data) {
        std::cout << "B(观察者): C的数据变化为: " << data << std::endl;
        last_c_data_ = data;
        
        // 基于数据变化执行相应逻辑
        if (data > 10) {
            std::cout << "B: C的数据已超过10，执行特殊处理" << std::endl;
        }
    }
    
    void workerThread() {
        while (running_.load()) {
            // B自己的处理逻辑
            if (a_ptr_) {
                int current_data = a_ptr_->getCData();
                if (current_data != last_c_data_) {
                    std::cout << "B(轮询): C的当前数据: " << current_data << std::endl;
                }
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};
*/

/*
// 方案3：共享数据模式
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <condition_variable>

class C {
private:
    int data_;
    mutable std::mutex mtx_;
    std::condition_variable cv_;

public:
    C() : data_(0) {}
    
    void setData(int new_data) {
        std::lock_guard<std::mutex> lock(mtx_);
        data_ = new_data;
        cv_.notify_all(); // 通知所有等待数据变化的线程
        std::cout << "C 数据设置为: " << data_ << std::endl;
    }
    
    int getData() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return data_;
    }
    
    // 等待数据达到特定值
    bool waitForData(int target, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mtx_);
        return cv_.wait_for(lock, timeout, [this, target]() {
            return data_ == target;
        });
    }
    
    // 等待数据变化
    bool waitForDataChange(int& new_data, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mtx_);
        int current = data_;
        if (cv_.wait_for(lock, timeout, [this, current]() {
            return data_ != current;
        })) {
            new_data = data_;
            return true;
        }
        return false;
    }
};

class SharedAccessController {
private:
    std::unique_ptr<C> c_ptr_;
    mutable std::mutex access_mtx_;
    std::atomic<bool> running_{false};
    std::thread updater_thread_;

public:
    SharedAccessController() : c_ptr_(std::make_unique<C>()) {}
    
    ~SharedAccessController() {
        stop();
    }
    
    void start() {
        if (running_.exchange(true)) return;
        
        updater_thread_ = std::thread(&SharedAccessController::updaterThread, this);
        std::cout << "共享访问控制器 开始运行" << std::endl;
    }
    
    void stop() {
        if (!running_.exchange(false)) return;
        
        if (updater_thread_.joinable()) {
            updater_thread_.join();
        }
        std::cout << "共享访问控制器 停止运行" << std::endl;
    }
    
    // 安全访问C的数据
    class SafeCAccess {
    private:
        C* c_ptr_;
        std::unique_lock<std::mutex> lock_;
        
    public:
        SafeCAccess(C* c, std::mutex& mtx) 
            : c_ptr_(c), lock_(mtx) {}
        
        bool isValid() const { return c_ptr_ != nullptr; }
        
        int getData() const { 
            return c_ptr_ ? c_ptr_->getData() : -1; 
        }
        
        void setData(int data) { 
            if (c_ptr_) c_ptr_->setData(data); 
        }
        
        bool waitForData(int target, std::chrono::milliseconds timeout) {
            return c_ptr_ ? c_ptr_->waitForData(target, timeout) : false;
        }
        
        bool waitForDataChange(int& new_data, std::chrono::milliseconds timeout) {
            return c_ptr_ ? c_ptr_->waitForDataChange(new_data, timeout) : false;
        }
    };
    
    SafeCAccess getSafeAccess() {
        return SafeCAccess(c_ptr_.get(), access_mtx_);
    }

private:
    void updaterThread() {
        int counter = 0;
        while (running_.load()) {
            {
                auto access = getSafeAccess();
                if (access.isValid()) {
                    access.setData(++counter);
                }
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
};

class A {
private:
    std::shared_ptr<SharedAccessController> controller_;

public:
    A(std::shared_ptr<SharedAccessController> controller) 
        : controller_(controller) {}
    
    void start() {
        if (controller_) {
            controller_->start();
        }
    }
    
    void stop() {
        if (controller_) {
            controller_->stop();
        }
    }
    
    // 获取控制器的共享访问
    std::shared_ptr<SharedAccessController> getController() const {
        return controller_;
    }
};

class B {
private:
    std::shared_ptr<SharedAccessController> controller_;
    std::atomic<bool> running_{false};
    std::thread worker_thread_;

public:
    B(std::shared_ptr<SharedAccessController> controller) 
        : controller_(controller) {}
    
    ~B() {
        stop();
    }
    
    void start() {
        if (running_.exchange(true)) return;
        
        worker_thread_ = std::thread(&B::workerThread, this);
        std::cout << "B 开始运行" << std::endl;
    }
    
    void stop() {
        if (!running_.exchange(false)) return;
        
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
        std::cout << "B 停止运行" << std::endl;
    }

private:
    void workerThread() {
        while (running_.load() && controller_) {
            // 使用安全访问
            auto access = controller_->getSafeAccess();
            if (access.isValid()) {
                int current_data = access.getData();
                std::cout << "B 安全访问C的数据: " << current_data << std::endl;
                
                // 等待数据变化
                int new_data;
                if (access.waitForDataChange(new_data, std::chrono::seconds(2))) {
                    std::cout << "B 检测到C数据变化: " << new_data << std::endl;
                }
            }
        }
    }
};
*/
int main() {
    std::cout << "=== 方案1: 接口代理模式测试 ===" << std::endl;
    {
        auto a = std::make_shared<A>();
        B b(a);
        
        a->start();
        b.start();
        
        std::this_thread::sleep_for(std::chrono::seconds(10));
        
        b.stop();
        a->stop();
    }
    
    // std::cout << "\n=== 方案2: 观察者模式测试 ===" << std::endl;
    // {
    //     auto a = std::make_shared<A>();
    //     B b(a);
        
    //     a->start();
    //     b.start();
        
    //     std::this_thread::sleep_for(std::chrono::seconds(10));
        
    //     b.stop();
    //     a->stop();
    // }
    
    // std::cout << "\n=== 方案3: 共享访问控制器测试 ===" << std::endl;
    // {
    //     auto controller = std::make_shared<SharedAccessController>();
    //     A a(controller);
    //     B b(controller);
        
    //     a.start();
    //     b.start();
        
    //     std::this_thread::sleep_for(std::chrono::seconds(10));
        
    //     b.stop();
    //     a.stop();
    // }
    
    std::cout << "所有测试完成" << std::endl;
    return 0;
}