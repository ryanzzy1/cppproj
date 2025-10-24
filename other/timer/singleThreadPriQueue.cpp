#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>

struct TimerTask {
    std::chrono::steady_clock::time_point expire_time;
    std::function<void()> callback_;
    int intervals_;
    int id;

    bool operator> (const TimerTask& other) const{
        return expire_time > other.expire_time;
    }
};

class TimerManager
{
public:

    TimerManager() : next_id_(0), running_(false) {}

    ~TimerManager() {stop();}

    int addTimer(int delay_ms, int interval_ms, std::function<void()> callback){
        auto task = TimerTask{
            std::chrono::steady_clock::now() + std::chrono::milliseconds(delay_ms),
            callback,
            interval_ms,
            next_id_++
        };

        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.push(task);
        }

        cv_.notify_one();

        return task.id;
    }

    void start() {
        running_ = true;
        worker_thread_ = std::thread(&TimerManager::run, this);
    }

    void stop() {
        running_ = false;
        cv_.notify_all();
        if(worker_thread_.joinable()) worker_thread_.join();
    }

private:

    void run(){
        while(running_) {
            std::unique_lock<std::mutex> lock(mutex_);
            if (tasks_.empty()) {
                cv_.wait(lock);
                continue;
            }

            auto next_task = tasks_.top();
            auto now = std::chrono::steady_clock::now();

            if(next_task.expire_time <= now) {
                tasks_.pop();
                lock.unlock();

                next_task.callback_();

                if (next_task.intervals_ > 0) {
                    auto new_task = next_task;
                    new_task.expire_time = now + std::chrono::microseconds(new_task.intervals_);
                    std::lock_guard<std::mutex> lock(mutex_);
                    tasks_.push(new_task);
                }

                lock.lock();
            } else {
                cv_.wait_until(lock, next_task.expire_time);
            }
        }
    }

    std::priority_queue<TimerTask, std::vector<TimerTask>, std::greater<TimerTask>> tasks_;

    std::mutex mutex_;

    std::condition_variable cv_;

    std::thread worker_thread_;

    std::atomic<bool> running_;

    int next_id_;

};

int main()
{
    TimerManager manager;
    
    manager.start();

    manager.addTimer(1000, 1000, [](){
        std::cout << "period timer: " << std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count() << "ms\n";
        
    });

    manager.addTimer(5000, 0, [](){std::cout << "single timer\n";});

    std::this_thread::sleep_for(std::chrono::seconds(10));

    return 0;
}