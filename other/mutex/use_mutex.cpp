#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <functional>

class TaskQueue
{
public:
    void push(int task) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            tasks_.push_back(task);
        }
        cond_.notify_one();
    }

    int pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] {return !tasks_.empty(); });
        int task = tasks_.front();
        tasks_.erase(tasks_.begin());
        return task;
    }

private:
    std::vector<int> tasks_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

std::once_flag flag;

void run_once() {
    std::cout << "Run once." << std::endl;
}

void worker(TaskQueue& taskQueue) {
    std::call_once(flag, run_once);

    while (true) {
        int task = taskQueue.pop();

        if (task == -1) {
            break;
        }

        std::cout << "Processing task: " << task << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    TaskQueue taskQueue;
    std::vector<std::thread> threads;

    for (int i = 0; i < 4; ++i) {
        threads.push_back(std::thread(worker, std::ref(taskQueue)));
    }

    for (int i = 0; i < 10; ++i) {
        taskQueue.push(i);
    }

    for (int i = 0; i < 4; ++i) {
        taskQueue.push(-1);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}