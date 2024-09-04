#include <chrono>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <thread>

using namespace std;

int main()
{
    // System_clock
    // format output current time method 1:
    /*
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::cout << "Current data and time: " << std::ctime(&now_c);
    // using <iomanip> and <ctime> format data and time
    std::cout << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S") << std::endl;
    */

    // format output current time mthod 2:
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm* ptm = std::localtime(&tt);
    std::cout << "Current time is: " << std::put_time(ptm, "%c") << std::endl;
    std::cout << "Current time is: " << std::put_time(ptm, "%y-%m-%d %H:%M:%S") << std::endl;

    // std::chrono::steady_clock
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::chrono::steady_clock::duration elapsed = end - start;
    long long elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    cout << "time duration: "<< elapsed_milliseconds << endl;

    // std::chrono::high_resolution_clock
    std::chrono::high_resolution_clock::time_point start2 = std::chrono::high_resolution_clock::now();
    int sum = 0;
    for (int i = 0; i < 1000000; i++)
        sum += i;
    cout << "sum: " << sum << endl;
    std::chrono::high_resolution_clock::time_point end2 = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::duration elapsed2 = end2 - start2;
    long long elapsed_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed2).count();
    cout << "time duration: "<< elapsed_microseconds << endl;

    // obain timestamp
    auto tt_now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(tt_now);
    std::tm* now_tm = std::localtime(&t);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now_tm);
    cout << "Current time: "<< buffer << endl;

    
    return 0;
}