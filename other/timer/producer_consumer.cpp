#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

std::mutex mtx_;
std::condition_variable cv_;

std::atomic<bool> flag1{false};
std::atomic<bool> flag2{false};
std::atomic<bool> flag3{false};
std::atomic<bool> flag4{false};

void producer_thread()
{
    int i = 0;
    while(i < 4){
        switch(i) {
            case 0 :
            {
                std::lock_guard<std::mutex> lock1(mtx_);
                if(!flag1){
                    flag1 = true;
                }
            }
                break;
            case 1:
                {
                    std::lock_guard<std::mutex> lock2(mtx_);
                    if(!flag2){
                        flag2 = true;
                    }
                }
                break;
            case 2:
                {
                    std::lock_guard<std::mutex> lock3(mtx_);
                    if(!flag3){
                        flag3 = true;
                    }
                }
                break;
            case 3:
                {
                    std::lock_guard<std::mutex> lock4(mtx_);
                    if(!flag4){
                        flag4 = true;
                    }
                }
                break;
            default:
                break;
        }
        cv_.notify_one();        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        i++;
        if ( i == 4) i = 0;
    }   
}

void consumer_thread()
{
    int i = 0;
    while(i < 4){
        switch(i){
            case 0:
            {
                std::unique_lock<std::mutex> lock1(mtx_);
                cv_.wait(lock1, [](){return flag1.load();});
                std::cout << "i = " << i << " This is Flag1 running\n";
                flag1.store("false");
            }                
                break;
            case 1:
            {
                std::unique_lock<std::mutex> lock2(mtx_);
                cv_.wait(lock2, [](){return flag2.load();});
                std::cout << "i = " << i << " This is Flag2 running\n";
                flag2.store("false");
            }
                break;
            case 2:
            {
                std::unique_lock<std::mutex> lock3(mtx_);
                cv_.wait(lock3, [](){return flag3.load();});
                std::cout << "i = " << i << " This is Flag3 running\n";
                flag3.store("false");
            }
                break;
            case 3:
            {
                std::unique_lock<std::mutex> lock4(mtx_);
                cv_.wait(lock4, [](){return flag4.load();});
                std::cout << "i = " << i << " This is Flag4 running\n";
                flag4.store("false");
            }
                break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        i++;
        if ( i == 4) i = 0;
    }
    
}

int main()
{

    std::thread th1(producer_thread);
    std::thread th2(consumer_thread);

    if(th1.joinable()){
        th1.join();
    }

    if(th2.joinable()){
        th2.join();
    }

    return 0;
}