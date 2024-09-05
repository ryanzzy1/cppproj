#include <iostream>
#include <atomic>
#include <thread>
/// @brief 
std::atomic<int> counter(0);

/* int counter = 0;

void increment() {
    for (int i = 0; i < 10000; i++) {
        ++counter;
    }
}
*/
void increment() {
    for (int i = 0; i < 10000; i++) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
}

// instruct resort

std::atomic<bool> ready(false);
std::atomic<int> data(0);
/*
// 未加内存屏障，可能导致指令重排，引起data数据未被即时更新
void producer() {
    data.store(42, std::memory_order_relaxed);
    ready.store(true, std::memory_order_relaxed);
}

void consumer() {
    while (!ready.load(std::memory_order_relaxed)) {
        std::this_thread::yield(); // do nothing, let cpu time slice palced

    }

    std::cout << data.load(std::memory_order_relaxed); // consumer use data
}*/

/** 添加内存屏障，保证数据多线程内存顺序 */
void producer() {
    data.store(42, std::memory_order_relaxed); //原子性更新data值，但不保证内存顺序
    ready.store(true, std::memory_order_relaxed); // 保证data的更新操作先于ready 更新操作
}

void consumer() {
    // 保证先读取ready的值，再读取data的值
    while (!ready.load(std::memory_order_acquire)) {
        std::this_thread::yield();      // do nothing， cpu 让出时间片
    }

    // ready 为true时，再原子性读取data值
    std::cout << data.load(std::memory_order_relaxed); // 消费者线程使用数据
}



int main()
{
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();

    std::thread t3(producer);
    std::thread t4(consumer);

    t3.join();
    t4.join();

    std::cout << "Final counter value: " << counter << std::endl;

    return 0;
}