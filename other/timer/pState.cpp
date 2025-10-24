#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <memory>

// P档状态枚举
enum class GearPosition {
    P,     // P档
    NON_P  // 非P档 (R, N, D等)
};

// CAN消息结构
struct CanMessage {
    uint32_t id;
    uint64_t timestamp;
    GearPosition gear_position;
    // 其他CAN数据...
};

// P档状态检测和定时器管理类
class GearMonitor {
public:
    GearMonitor() 
        : timer_active_(false),
          current_gear_(GearPosition::P),
          last_gear_(GearPosition::P),
          monitoring_active_(false) {}
    
    ~GearMonitor() {
        stopMonitoring();
    }
    
    // 开始监控
    void startMonitoring() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (monitoring_active_) return;
        
        monitoring_active_ = true;
        current_gear_ = GearPosition::P;
        last_gear_ = GearPosition::P;
        
        // 启动5分钟定时器
        start5MinuteTimer();
        
        std::cout << "开始P档状态监控，定时器5分钟..." << std::endl;
    }
    
    // 停止监控
    void stopMonitoring() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            monitoring_active_ = false;
        }
        stopTimer();
        std::cout << "停止P档状态监控" << std::endl;
    }
    
    // 处理接收到的CAN消息
    void processCanMessage(const CanMessage& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!monitoring_active_) return;
        
        // 更新档位状态
        last_gear_ = current_gear_;
        current_gear_ = msg.gear_position;
        
        // 检测P->非P的状态变化
        if (last_gear_ == GearPosition::P && current_gear_ == GearPosition::NON_P) {
            std::cout << "检测到P档->非P档变化，重置5分钟定时器" << std::endl;
            resetTimer();
        }
        
        // 可选：记录状态变化
        logGearStatus();
    }
    
    // 设置定时器超时回调函数
    void setTimerCallback(std::function<void()> callback) {
        timer_callback_ = callback;
    }
    
    // 获取当前监控状态
    bool isMonitoring() const {
        return monitoring_active_.load();
    }
    
    // 获取剩余时间（用于调试或显示）
    std::chrono::milliseconds getRemainingTime() const {
        return remaining_time_.load();
    }

private:
    // 启动5分钟定时器
    void start5MinuteTimer() {
        stopTimer();
        timer_active_ = true;
        
        timer_thread_ = std::thread([this]() {
            auto timeout = std::chrono::minutes(5);
            auto start_time = std::chrono::steady_clock::now();
            
            while (timer_active_.load()) {
                std::unique_lock<std::mutex> lock(timer_mutex_);
                
                // 计算剩余时间
                auto elapsed = std::chrono::steady_clock::now() - start_time;
                auto remaining = timeout - elapsed;
                remaining_time_.store(std::chrono::duration_cast<std::chrono::milliseconds>(remaining));
                
                if (remaining <= std::chrono::milliseconds(0)) {
                    // 定时器超时
                    if (timer_callback_) {
                        std::cout << "5分钟定时器超时，执行后续逻辑" << std::endl;
                        timer_callback_();
                    }
                    timer_active_ = false;
                    break;
                }
                
                // 等待100ms检查一次，同时支持被提前唤醒（用于重置）
                if (timer_cv_.wait_for(lock, std::chrono::milliseconds(100), 
                                      [this]() { return !timer_active_.load(); })) {
                    break;
                }
            }
        });
    }
    
    // 重置定时器
    void resetTimer() {
        std::lock_guard<std::mutex> lock(timer_mutex_);
        if (timer_active_.load()) {
            timer_cv_.notify_all(); // 唤醒定时器线程以重新开始
            
            // 重新启动定时器线程
            if (timer_thread_.joinable()) {
                timer_thread_.join();
            }
            start5MinuteTimer();
            
            std::cout << "定时器已重置，重新开始5分钟计时" << std::endl;
        }
    }
    
    // 停止定时器
    void stopTimer() {
        {
            std::lock_guard<std::mutex> lock(timer_mutex_);
            timer_active_ = false;
        }
        timer_cv_.notify_all();
        if (timer_thread_.joinable()) {
            timer_thread_.join();
        }
    }
    
    // 记录档位状态（可选，用于调试）
    void logGearStatus() {
        static auto last_log_time = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        
        // 每5秒记录一次状态，避免日志过多
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_log_time).count() >= 5) {
            std::cout << "当前档位: " << (current_gear_ == GearPosition::P ? "P档" : "非P档")
                      << ", 剩余时间: " << getRemainingTime().count() / 1000 << "秒" << std::endl;
            last_log_time = now;
        }
    }
    
private:
    std::atomic<bool> timer_active_;
    std::atomic<bool> monitoring_active_;
    std::atomic<std::chrono::milliseconds> remaining_time_;
    
    GearPosition current_gear_;
    GearPosition last_gear_;
    
    std::thread timer_thread_;
    std::mutex mutex_;
    std::mutex timer_mutex_;
    std::condition_variable timer_cv_;
    
    std::function<void()> timer_callback_;
};

// CAN消息模拟器（用于测试）
class CanSimulator {
public:
    CanSimulator(GearMonitor& monitor) : monitor_(monitor), running_(false) {}
    
    void start() {
        running_ = true;
        sim_thread_ = std::thread([this]() {
            int cycle_count = 0;
            GearPosition current_sim_gear = GearPosition::P;
            
            while (running_.load()) {
                // 每100ms生成一个CAN消息
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                
                // 模拟档位变化：每30秒切换一次档位状态
                cycle_count++;
                if (cycle_count % 300 == 0) { // 300 * 100ms = 30秒
                    current_sim_gear = (current_sim_gear == GearPosition::P) ? 
                                      GearPosition::NON_P : GearPosition::P;
                    std::cout << "[模拟器] 档位切换到: " 
                              << (current_sim_gear == GearPosition::P ? "P档" : "非P档") << std::endl;
                }
                
                // 创建CAN消息并处理
                CanMessage msg;
                msg.id = 0x123;
                msg.timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
                msg.gear_position = current_sim_gear;
                
                monitor_.processCanMessage(msg);
            }
        });
    }
    
    void stop() {
        running_ = false;
        if (sim_thread_.joinable()) {
            sim_thread_.join();
        }
    }
    
private:
    GearMonitor& monitor_;
    std::atomic<bool> running_;
    std::thread sim_thread_;
};


// 测试主函数
int main() {
    std::cout << "=== P档状态监控系统测试 ===" << std::endl;
    
    // 创建监控器
    GearMonitor monitor;
    
    // 设置定时器超时回调
    monitor.setTimerCallback([]() {
        std::cout << "*** 5分钟内未检测到P->非P变化，执行后续业务逻辑 ***" << std::endl;
        // 这里可以添加您的业务逻辑
    });
    
    // 创建CAN模拟器
    CanSimulator simulator(monitor);
    
    // 开始监控
    monitor.startMonitoring();
    
    // 启动CAN模拟
    simulator.start();
    
    // 让测试运行一段时间（比如10分钟）
    std::cout << "测试运行中...（10分钟后自动结束）" << std::endl;
    std::this_thread::sleep_for(std::chrono::minutes(10));
    
    // 停止测试
    simulator.stop();
    monitor.stopMonitoring();
    
    std::cout << "测试结束" << std::endl;
    return 0;
}

// 实际应用中的使用示例
class VehicleSystem {
public:
    VehicleSystem() {
        // 设置定时器回调
        gear_monitor_.setTimerCallback([this]() {
            onGearStable();
        });
    }
    
    void startSystem() {
        std::cout << "启动车辆系统..." << std::endl;
        gear_monitor_.startMonitoring();
        // 启动CAN接收等其他组件
    }
    
    void stopSystem() {
        gear_monitor_.stopMonitoring();
        std::cout << "停止车辆系统..." << std::endl;
    }
    
    // CAN消息接收处理（在实际系统中由CAN驱动调用）
    void onCanMessageReceived(const CanMessage& msg) {
        gear_monitor_.processCanMessage(msg);
    }
    
private:
    void onGearStable() {
        std::cout << "P档状态已稳定5分钟，执行后续操作..." << std::endl;
        // 这里执行您的业务逻辑
        // 例如：允许启动特定功能、发送特定指令等
    }
    
private:
    GearMonitor gear_monitor_;
};