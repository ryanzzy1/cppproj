#include <iostream>
#include <chrono>
#include <thread>

class Timer
{
    private:
        bool running;
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time, end_time;
    public:
        Timer() : running(false) {}

        void start() {
            running = true;
            start_time = std::chrono::high_resolution_clock::now();
        }

        void stop() {
            if (running) {
                end_time = std::chrono::high_resolution_clock::now();
                running = false;
            }
        }

        double elapsed() {
            if (running) {
                return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start_time).count();
            }
            else {
                return std::chrono::duration<double>(end_time - start_time).count();
            }
        }

        void reset() {
            running = false;
        }
};

int main() {
    Timer timer;
    timer.start();
    int sum = 0;
    for (int i = 0; i < 10000000; i++)
        sum += i;
    std::cout << "sum=" << sum << std::endl;
    timer.stop();
    std::cout << "Elapsed time: " << timer.elapsed() << " seconds." << std::endl;
    // timer continue
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    timer.stop();
    std::cout << "Total elapsed time: " << timer.elapsed() << " seconds." << std::endl;

    return 0;
}