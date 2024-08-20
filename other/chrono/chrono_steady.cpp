#include <chrono>
#include <iostream>
#include <cstddef>
#include <iomanip>
#include <numeric>
#include <vector>

using namespace std;


volatile int sink;
void do_some_work(std::size_t size)
{
    std::vector<int> v(size, 42);
    sink = std::accumulate(v.begin(), v.end(), 0);
}


int main()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    cout << "time now: " << std::ctime(&tt);

    // static std::chrono::time_point<std::chrono::steady_clock>now() noexcept;
    cout << std::fixed << std::setprecision(9) << std::left;
    for (auto size{1ull}; size < 10'00'00'00'00ull; size *= 100)
    {
        const auto start = std::chrono::steady_clock::now();
        do_some_work(size);
        const auto end = std::chrono::steady_clock::now();

        const std::chrono::duration<double> diff = end - start;
        std::cout << "fill with : " << std::setw(9)
             << size << " number int's vector need :" << diff.count() << "\n" ;
        
    }

}