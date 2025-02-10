#include <iostream>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <chrono>

std::atomic<bool> stop_(false);
std::mutex mtx_;
std::condition_variable cv_;

void test()
{
    
}


bool is_stopped()
{
    return stop_.load();
}

void wait_for_rep()
{
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait_for(lock, std::chrono::milliseconds(1000), [&]()
            {
                return is_stopped();
            });
    std::cout << "entering wait_for_rep.\n";
}

int main()
{
    // stop_.store(true);
    if(!is_stopped()){
        std::cout << "entering main waiting.\n";
        // stop_.store(true);

        wait_for_rep();

    }

}