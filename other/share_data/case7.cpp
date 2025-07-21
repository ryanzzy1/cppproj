#include <mutex>
#include <atomic>
#include <thread>
#include <iostream>

class ThreadSafeData {
    std::mutex mtx;
    int value = 0;
public:
    int safeIncrement() {
        std::lock_guard<std::mutex> lock(mtx);
        ++value;
        // 返回当前值
        std::cout << "Current value: " << value << std::endl;
        // 或者返回值
        return value;
    }

    // 或使用原子操作
    std::atomic<int> counter{0};
    void atomicAdd() {
        counter++;
    }
};

#include <iostream>

int main() {
    ThreadSafeData data;

    // std::thread t1([&](){data.safeIncrement();});
    std::thread t2([&]{ data.safeIncrement(); });

    return 0;
}