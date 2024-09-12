#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mutex_;
std::condition_variable condition_;

int num = 0;
bool shipment() {
    return num != 0;
}

void consumer(int n) {
    for (int i = 0; i < n; ++i) {
        std::unique_lock<std::mutex> lck(mutex_);
        condition_.wait(lck, shipment); // 每次都会阻塞，等待主线程通知唤醒
        std::cout <<"consumer thread: "<< i << " num: " <<num << "\n";
        num = 0;
    }
}

int main() {
    std::thread t1(consumer, 10);

    for (int i = 0; i < 10; ++i) {
        while (shipment())
            std::this_thread::yield(); // 让出cpu时间，减少CPU占用, 此时while处还在执行
        std::cout << "main thread: " << i << std::endl;
        std::unique_lock<std::mutex> lck(mutex_);
        num = i + 1;
        condition_.notify_one();    
    }

    t1.join();

    return 0;    
}