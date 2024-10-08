#include <iostream>
#include <signal.h>
#include <thread>
#include <chrono>

void signal_handler(int signal_num) {
    std::cout << "Received signal: " << signal_num << std::endl;
}

int main()
{
    signal(SIGINT, signal_handler);

    while(1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}