#include <iostream>
#include <csignal>
#include <functional>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <atomic>

std::function<void(int)> stop_signal_handler;

void signal_handler(int sig) {
    stop_signal_handler(sig);
}

// Register the signal handler
// This function will be called when the signal is received
// The signal handler will call the stop_signal_handler function
// The stop_signal_handler function will be defined later

int main(){

    std::cout << "signal processing" << std::endl;
    stop_signal_handler = [&](int sig) {
        std::cout << "Received signal: " << sig << std::endl;
        exit(0);
            };

    int count = 0;
    while(count < 10) {
        // Simulate some work
        std::cout << "Working..." << std::endl;
        // Sleep for a second
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // Increment the count
        // This is just to simulate some work being done
        // In a real application, you would replace this with your actual work
        // For example, you might be processing data or waiting for user input
        // In this case, we are just incrementing a counter
        // to simulate some work being done
        // In a real application, you would replace this with your actual work

        std::cout << "count: " << count << std::endl;
        count++;
    }
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
}