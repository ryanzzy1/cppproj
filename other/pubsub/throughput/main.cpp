#include <iostream>
// #include <cstdint>
#include <cstring>
// #include <iomanip>
// #include <bitset>
#include <map>
#include <vector>
#include <chrono>


// strtol
#include <cerrno>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <limits>
// strtol end

typedef struct alignas(4) ThroughputType
{
    // identifies the sample sent
    uint32_t seqnum;
    // actual payload
    uint8_t data[1];
    // This struct overhead
    static const size_t overhead;

} ThroughputType;

enum TestAgent
{
    PUBLISHER,
    SUBSCRIBER,
    BOTH
};

int main(int argc, char** argv)
{
    int columns = getenv("COLUMNS") ? atoi(getenv("COLUMNS")) : 80;

    std::cout << "clolumns: " << columns << std::endl;
    TestAgent test_agent = TestAgent::PUBLISHER;
    uint32_t test_time_sec = 5;
    uint32_t recovery_time_ms = 10;
    int demand = 10000;
    uint32_t msg_size = 1024;   // 1KB, 1MB = 1048576B
    bool reliable = false;  // defalut udp transport
    uint32_t seed = 80; 

    argc -= (argc > 0);
    argv += (argc > 0);
    std::cout << "argc: " << argc << " argv: " << *argv << std::endl;
    argc -= (argc > 0); argv += (argc > 0);
    std::cout << "argc: " << argc << " argv: " << *argv << std::endl;


    if (argc)
    {
        if (strcmp(argv[0], "publisher") == 0)
        {
            test_agent = TestAgent::PUBLISHER;
        }
        else if (strcmp(argv[0], "subscriber") == 0)
        {
            test_agent = TestAgent::SUBSCRIBER;
        }
        else if (strcmp(argv[0], "both") == 0)
        {
            test_agent = TestAgent::BOTH;
        }
        else
        {
            std::cout << "usage: \n"
                      << "-h    --help  produce this message."
                      << ""
                      << " " << std::endl;
        }
        if (0 == strncasecmp("onon", "onon", 4)) {
            std::cout << "ON\n";
        }

        char buffer[20] = "112233abc11";
        char *endptr;
        std::cout << "initial buffer:" << buffer << std::endl;
        long result = strtol(buffer, &endptr, 2);
        std::cout << "convert to result:" << result << "\t, left string endptr: " << endptr << std::endl;
        // std::thread pub_thread();
        // auto seed = strtol(NULL, nullptr, 10);
        // std::cout << "seed : " << seed << "\n";
        // for(int i = 0; i < 10; i++)
        // {
        //     std::cout << " i= " << ++i << std::endl;
        // }
    }
    
    uint32_t test_seq = 2;
    uint32_t test_num = 3;

    std::cout << "test: " << (test_num > test_seq + 1) << std::endl;

    const char* p = "10 200000000000000000000000000000 30 -40";
    std::cout << "解析 " << std::quoted(p) << ":\n";
 
    for (;;)
    {
        errno = 0;
        char* p_end{};
        const long i = std::strtol(p, &p_end, 10);
        std::cout << "i:" << i << " left string chracter:" << p_end << std::endl;
        if (p == p_end)
            break;

        const bool range_error = errno == ERANGE;
        const std::string extracted(p, p_end - p);
        p = p_end;
 
        std::cout << "提取到 " << std::quoted(extracted)
                  << ", strtol 返回 " << i << '.';
        if (range_error)
            std::cout << " 发生值域错误.";
 
        std::cout << '\n';
    }

    // std::map<uint32_t, std::vector<uint32_t>> demand_payload_;

    // for (auto sit = demand_payload_.begin(); sit != demand_payload_.end(); ++sit)
    // {
        
    // }

    std::vector<uint32_t> recovery_times_ = {10,20};
    std::cout << "recovery_times_.size(): " << recovery_times_.size() << std::endl;
    for (uint16_t i = 0; i < recovery_times_.size(); i++)
    {
        std::cout << "recovery_times_: " << recovery_times_[i] << ", ";
    }
    std::cout << std::endl;

    auto time = std::numeric_limits<uint64_t>::min();

    auto time_e = std::numeric_limits<uint64_t>::max();

    std::cout << "time: " << time << "time_e: " << time_e << std::endl;
    std::chrono::duration<double, std::micro> x_us = std::chrono::milliseconds(1);
    std::cout << "x_ns: " << x_us.count() << std::endl;


    std::cout << "setprecision:" << std::fixed << std::setprecision(3) << 1234.123456 << std::endl;

    return 0;
}