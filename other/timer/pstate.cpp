#include <cstdint>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>

// P档状态定义（从CAN信号解析）
enum class PState {
    P = 0,         // P档
    NON_P = 1      // 非P档（如D/R/N档）
};

// 定时器状态
enum class TimerState {
    STOPPED,       // 未运行
    RUNNING,       // 运行中
    EXPIRED        // 已到期（5分钟无跳变）
};



class PstateMonitor {
private:
    // CAN信号相关
    PState current_p_state_;       // 当前P档状态
    PState last_p_state_;          // 上一次P档状态（用于跳变检测）

    // 定时器相关
    std::atomic<TimerState> timer_state_;  // 定时器状态（线程安全）
    std::atomic<uint64_t> remaining_ms_;   // 剩余时间（ms）
    uint64_t total_duration_ms_;           // 总定时时间（5分钟=300000ms）
    std::thread timer_thread_;             // 定时器线程
    std::mutex state_mutex_;               // 保护状态变量的互斥锁

    // 后续逻辑的回调函数（定时器到期后执行）
    std::function<void()> on_timeout_callback_;

    // 私有函数：解析CAN信号（模拟，实际需从CAN报文提取）
    PState parseCanSignal(const uint8_t* can_data) {
        // 假设CAN信号中第0位表示P档状态（0=P，1=非P）
        return (can_data[0] & 0x01) ? PState::NON_P : PState::P;
    }

    // 私有函数：检测P→非P跳变
    bool checkPToNonPJump() const {
        return (last_p_state_ == PState::P) && (current_p_state_ == PState::NON_P);
    }

    // 私有函数：定时器线程逻辑
    void timerLoop() {
        auto start_time = std::chrono::steady_clock::now();
        while (timer_state_ == TimerState::RUNNING) {
            // 计算已过去的时间
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start_time
            ).count();

            // 检查是否超时
            if (elapsed >= total_duration_ms_) {
                timer_state_ = TimerState::EXPIRED;
                if (on_timeout_callback_) {
                    on_timeout_callback_();  // 执行后续逻辑
                }
                break;
            }

            // 检查是否需要重置（由外部跳变触发）
            std::unique_lock<std::mutex> lock(state_mutex_);
            if (remaining_ms_ != total_duration_ms_) {
                // 重置定时器：更新起始时间，恢复剩余时间
                start_time = std::chrono::steady_clock::now();
                remaining_ms_ = total_duration_ms_;
            }
            lock.unlock();

            // 100ms检测一次（与CAN周期一致）
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

public:
    // 构造函数：初始化定时时间（默认5分钟）
    PstateMonitor(uint64_t duration_ms = 30000) 
        : total_duration_ms_(duration_ms),
          current_p_state_(PState::P),
          last_p_state_(PState::P),
          timer_state_(TimerState::STOPPED),
          remaining_ms_(duration_ms) {}

    // 析构函数：确保线程正确停止
    ~PstateMonitor() {
        stopMonitor();
    }

    // 设置定时器到期后的回调函数（后续逻辑）
    void setTimeoutCallback(std::function<void()> callback) {
        on_timeout_callback_ = callback;
    }

    // 处理接收到的CAN报文（每100ms被调用一次）
    void processCanMessage(const uint8_t* can_data) {
        std::unique_lock<std::mutex> lock(state_mutex_);

        // 更新状态历史
        last_p_state_ = current_p_state_;
        // 解析新状态
        current_p_state_ = parseCanSignal(can_data);

        // 检测P→非P跳变：若发生，重置定时器
        if (checkPToNonPJump()) {
            resetTimer();
        }

        lock.unlock();
    }

    // 重置定时器（重新开始5分钟计时）
    void resetTimer() {
        if (timer_state_ == TimerState::RUNNING) {
            remaining_ms_ = total_duration_ms_;  // 触发timerLoop中的重置逻辑
        }
    }

    // 启动监控（开始接收CAN并启动定时器）
    void startMonitor() {
        if (timer_state_ == TimerState::STOPPED) {
            timer_state_ = TimerState::RUNNING;
            // 启动定时器线程
            timer_thread_ = std::thread(&PstateMonitor::timerLoop, this);
        }
    }

    // 停止监控
    void stopMonitor() {
        if (timer_state_ == TimerState::RUNNING) {
            timer_state_ = TimerState::STOPPED;
            if (timer_thread_.joinable()) {
                timer_thread_.join();
            }
        }
    }
};


// 后续逻辑：5分钟无跳变后执行
void executeAfterTimeout() {
    printf("5分钟内未检测到P→非P跳变，执行后续逻辑（如允许上电/启动）...\n");
}

// 模拟CAN信号发送（每100ms发送一次P档状态）
void simulateCanSender(PstateMonitor& monitor) {
    // 模拟状态序列：P→P→...→P→非P→非P→...（3分钟时触发一次跳变）
    uint8_t can_data[8] = {0x00}; // 初始为P档（0x00表示P）
    for (int i = 0; i < 3000; ++i) {  // 3000*100ms=300000ms=5分钟
        // 第3分钟（1800*100ms）时切换为非P档
        if (i == 1800) {
            can_data[0] = 0x01; // 非P档
            printf("CAN信号：P→非P（触发定时器重置）\n");
        }
        // 发送CAN信号给监控器
        monitor.processCanMessage(can_data);
        // 每100ms发送一次
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    
    
    // 创建监控器（5分钟定时）
    PstateMonitor monitor;
    // 设置超时回调（后续逻辑）
    monitor.setTimeoutCallback(executeAfterTimeout);
    // 启动监控
    monitor.startMonitor();
    printf("开始监控P档状态，5分钟内无P→非P跳变将执行后续逻辑...\n");

    // 模拟CAN信号发送（独立线程）
    std::thread can_sender(simulateCanSender, std::ref(monitor));
    can_sender.join();

    // 停止监控
    monitor.stopMonitor();
    return 0;
}