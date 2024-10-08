#include <iostream>
#include <csignal>
#include <functional>

std::function<void(int)> stop_signal_handler;

void signal_handler(int sig) {
    stop_signal_handler(sig);
}

int main(){

    std::cout << "signal";
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
}