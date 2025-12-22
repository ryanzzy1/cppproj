#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <ctime>
#include <string>
#include <memory>
#include <functional>


static int global_var = 10;

constexpr int compile_time_value = 20;

static constexpr int static_compile_var = 30;

class Example
{
public:
    static int new_value;
    static constexpr int class_value = 50;
};

enum class StatusFlag : std::uint8_t
{
    kTimeOut = 0x0, 
    kSynchronized = 0x1, 
    kSynchToGateway = 0x2, 
    kTimeLeapFuture = 0x3, 
    kTimeLeapPast = 0x4, 
    kHasDLS = 0x5, 
    kDLSActive = 0x6 
};

std::vector<StatusFlag> Clock_Data;

std::string specifier{"RPortPrototype"};
int Example::new_value = 100;

int main()
{
    // Example::new_value = 300;

    std::cout << "Global variable: " << global_var << std::endl;
    global_var = 100;
    std::cout << "Global variable modified: " << global_var << std::endl;

    std::cout << "Compile-time constant: " << compile_time_value << std::endl;
    // compile_time_value = 200; // read-only 不可修改

    std::cout << "static_compile_var: " << static_compile_var << std::endl;

    std::cout << "Example::class_value: " << Example::class_value << std::endl;

    std::cout << "Example::new_value: " << Example::new_value << std::endl;

    Clock_Data.push_back(StatusFlag::kSynchronized);

    std::cout << "Clock_Data size: " << Clock_Data.size() << std::endl;

    auto time_not_setyet = std::chrono::steady_clock::now().time_since_epoch().count();

    auto now = std::chrono::system_clock::now();
    
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    std::string inst_specifier{specifier.begin(), specifier.end()};

    std::cout << "inst_specifier: " << inst_specifier << std::endl;

    std::tm* now_tm = std::localtime(&now_time_t);
    std::tm* now_tm_t = std::gmtime(&now_time_t);
    std::cout << "YYY---MMM---DDD---HHH---MMM---SSS:\n " 
              << (now_tm->tm_year + 1900) << "-" 
              << (now_tm->tm_mon + 1) << "-" 
              << now_tm->tm_mday << " " 
              << now_tm->tm_hour << ":" 
              << now_tm->tm_min << ":" 
              << now_tm->tm_sec << std::endl;   

        std::cout << "YYY---MMM---DDD---HHH---MMM---SSS:\n " 
              << (now_tm_t->tm_year + 1900) << "-" 
              << (now_tm_t->tm_mon + 1) << "-" 
              << now_tm_t->tm_mday << " " 
              << now_tm_t->tm_hour << ":" 
              << now_tm_t->tm_min << ":" 
              << now_tm_t->tm_sec << "--"
              << now_tm_t->tm_wday << "-"
              << now_tm_t->tm_yday << "-"
              << now_tm_t->tm_isdst
              << std::endl;   

    std::cout << "time_not_setyet: " << time_not_setyet << std::endl;




    return 0;
}