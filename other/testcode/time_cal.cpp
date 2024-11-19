#include <iostream>
#include <chrono>



int main(){
    std::chrono::steady_clock::time_point end_time;
    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
    for (int i = 0; i < 1000; ++i)
    {
        end_time = std::chrono::steady_clock::now();
    }

    std::chrono::duration<double, std::micro> overhead_time = std::chrono::duration<double, std::micro>(end_time - start_time) / 1001;
    std::cout << "Overhead " << overhead_time.count() << " us" << std::endl;
}