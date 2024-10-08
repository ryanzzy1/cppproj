#include <iostream>
#include <atomic>

int main()
{
    std::atomic<bool> stop_(false);
    std::cout << stop_ << "\n";
    
    stop_.store(true);
    std::cout << stop_ << "\n";

    stop_.load();
    std::cout << stop_ << "\n";
}