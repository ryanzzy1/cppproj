#include <iostream>
#include <signal.h>
#include <atomic>
#include <chrono>
#include <thread>

void signalhandler(int sig){
    std::cout << "HelloWorld" << " Sig=" << sig << std::endl;


}

int main()
{

    std::cout << "Before Sig" << std::endl;
    signal(53, signalhandler);

    std::this_thread::sleep_for(std::chrono::milliseconds(100000));
    std::cout << "After Sig" << std::endl;

    return 0;
}
