#include <chrono>
#include <iostream>
#include <iomanip>


int main()
{
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);

    std::tm tm_now;

    localtime_r(&time_t_now, &tm_now);

    std::ostringstream oss;
    oss << std::put_time(&tm_now, "%Y-%m-%dT%H:%M:%S");
    // std::cout << time_t_now << "\n";

    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
    oss << '.' << std::setfill('0') << std::setw(3) << milliseconds;

    std::cout << oss.str() << "\n";

    return 0;
}