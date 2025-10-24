#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
using namespace std::chrono_literals;
 

std::mutex mtx;
std::condition_variable cv_;
int i;

void lck_reg(int idx){

    std::unique_lock<std::mutex> lck(mtx);
    cv_.wait_for(lck, idx*500ms, []{return i == 1;});


    std::cout << "releas lck_reg!" << std::endl;

}

void notify_reg()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    std::cerr << "waiting for ...\n";
    // cv_.notify_one();
    cv_.notify_all();
    std::this_thread::sleep_for(100ms);
    {
        std::lock_guard<std::mutex> lk(mtx);
        i = 1;
    }
    std::cerr << "Notifying again...\n";
    cv_.notify_all();

    // std::cout << "notify_reg finished!" << std::endl;
}

int main()
{

    std::thread t1(lck_reg, 1), t2(lck_reg, 2), t3(lck_reg, 3), t4(notify_reg);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}