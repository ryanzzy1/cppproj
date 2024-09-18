#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;
int shared_resource = 0;

void increment() {
    for (int i = 0; i < 10000; ++i) {
        // std::lock_guard<std::mutex> lock(mtx);
        std::unique_lock<std::mutex> lock(mtx);
        // mtx.lock();
        ++shared_resource;
        // mtx.unlock();
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();

    std::cout << "Final value of shared_resource: " << shared_resource << std::endl;

    return 0;
}