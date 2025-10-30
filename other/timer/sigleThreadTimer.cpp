#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <vector>
#include <atomic>
#include <memory>

class SimpleTimer
{

private:
    int intervals_;

    std::function<void()> callback_;

    std::atomic<bool> active_;

    bool repeat_;

    std::thread thread_;

public:
    SimpleTimer(int intervals, std::function<void()> callback, bool repeat = true)
        : intervals_(intervals), callback_(callback), repeat_(repeat), active_(false)
    {
    }

    ~SimpleTimer()
    {

        stop();
    }

    void start()
    {

        active_ = true;

        thread_ = std::thread([this]()
                              {
            while(active_){
                std::this_thread::sleep_for(std::chrono::milliseconds(intervals_));
                if(active_) callback_();
                if(!repeat_) active_ = false;
            } });
    }

    void stop()
    {
        active_ = false;
        if (thread_.joinable())
            thread_.join();
    }
};

int main()
{
    // std::vector<std::unique_ptr<SimpleTimer>> timers;

    // timers.push_back(std::make_unique<SimpleTimer>(1000, []()
    //                                                { std::cout << "This is timer interval 1s." << std::endl; }));

    // timers.push_back(std::make_unique<SimpleTimer>(5000, []()
    //                                                { std::cout << "This is timer interval 5s, running once." << std::endl; }, false));

    // for (auto &timer : timers)
    // {
    //     timer->start();
    // }

    // std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    // timers.clear();

    std::unique_ptr<SimpleTimer> tttimer = std::make_unique<SimpleTimer>(1000, []()
                                                                         { std::cout << "This is tttimer interval 1s." << std::endl; });
    tttimer->start();

    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    // tttimer->stop();

    return 0;
}