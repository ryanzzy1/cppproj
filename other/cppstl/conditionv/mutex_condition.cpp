#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>
#include <functional>


std::mutex run_mutex_;
std::condition_variable run_condition_;
std::mutex main_mutex_;
std::condition_variable main_condition_;
bool blocked_ = false;
bool running_ = true;

bool is_offered = false;

void run() {
    std::unique_lock<std::mutex> its_lock(run_mutex_);
    while(!blocked_)
        run_condition_.wait(its_lock);
        std::cout << "condition.wait Finished." << std::endl;
    
    while(running_) {
        std::cout << "case: " << running_;
        for (int i = 0; i < 10 && running_; i++){
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        }
    }
}

void notify(){
    while(running_) {
        std::unique_lock<std::mutex> its_lock(main_mutex_);
        while(!is_offered && running_)
            main_condition_.wait(its_lock);
            std::cout << "main_condition_.wait Finished." << std::endl;
        while(is_offered && running_) {
            
            for (uint32_t i = 0; i < 5; ++i)
                std::cout << "data: "<< static_cast<uint8_t>(i) << std::endl;
            {

            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}


int main()
{
    run_condition_.notify_one();
    main_condition_.notify_one();

    std::thread t1(run);
    std::thread t2(notify);

    if(t1.joinable()) 
        t1.join();
    if(t2.joinable())
        t2.join();

    return 0;    
}