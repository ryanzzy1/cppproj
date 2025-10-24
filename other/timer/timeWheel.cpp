#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include <chrono>
#include <atomic>
#include <thread>

class TimeWheel{
    
public:
    TimeWheel(int slots, int interval_ms)
    : slots_(slots), interval_ms_(interval_ms), current_slot_(0), running_(false){
        wheel_.resize(slots_);
    }

    void addTimer(int delay_ms, std::function<void()>callback){
        int ticks = (delay_ms + interval_ms_ - 1) / interval_ms_;
        int slot = (current_slot_ + ticks) % slots_;

        wheel_[slot].push_back(callback);
    }

    void start() {
        running_ = true;
        worker_thread_ = std::thread([this](){
            while (running_)
            {
                auto start = std::chrono::steady_clock::now();

                for(auto& callback : wheel_[current_slot_]) {
                    callback();
                }
                wheel_[current_slot_].clear();

                current_slot_ = (current_slot_ + 1) % slots_;

                auto end = std::chrono::steady_clock::now();
                
                auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                auto sleep_time = std::chrono::milliseconds(interval_ms_) - elapsed;

                if(sleep_time.count() > 0) {
                    std::this_thread::sleep_for(sleep_time);
                }
            } 
        });
    }

    void stop() {
        running_ = false;
        if (worker_thread_.joinable()) worker_thread_.join();
    }

    ~TimeWheel() {stop();}

private:

    int slots_;
    
    int interval_ms_;
    
    int current_slot_;
    
    std::atomic<bool> running_;
    
    std::vector<std::list<std::function<void()>>> wheel_;
    
    std::thread worker_thread_;

};