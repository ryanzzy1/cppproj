#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include <bits/stl_algo.h>



using namespace std;
std::vector<std::chrono::duration<double, std::micro>> times_;
class TimeStats
{
public:

    TimeStats()
        : bytes_(0)
        , received_(0)
        , minimum_(0)
        , maximum_(0)
        , percentile_50_(0)
        , percentile_90_(0)
        , percentile_99_(0)
        , percentile_9999_(0)
        , mean_(0)
        , stdev_(0)
    {
    }

    ~TimeStats()
    {
    }

    uint64_t bytes_;
    unsigned int received_;
    std::chrono::duration<double, std::micro> minimum_;
    std::chrono::duration<double, std::micro> maximum_;
    double percentile_50_;
    double percentile_90_;
    double percentile_99_;
    double percentile_9999_;
    double mean_;
    double stdev_;
};
TimeStats stats;

int main ()
{
    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();;
    
    double base, exponent, result;
    base = 3.0;
    exponent = 4.0;
    for(int i = 0; i < 10; ++i){
        result = pow(base, exponent);    
        cout << base << "^" << exponent << " = " << result << endl;
        base++;
        // exponent++;
        std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
        times_.push_back(end_time - start_time);
    }

    sort(times_.begin(), times_.end());
    for (auto tit = times_.begin(); tit != times_.end(); ++tit){
        cout << "++tit:" << times_.size() <<" -- times: " << (*tit).count() << endl;
    }
    size_t elem = 0;
    cout << "size: " << times_.size() << endl;
    elem = static_cast<size_t>(times_.size() * 0.5);
     cout << "elem: " << elem << endl;
    if (elem > 0 && elem <= times_.size())
    {
        // cout << "--elem: " << elem << "--" << times_.at(--elem).count() << endl;
        stats.percentile_50_ = times_.at(--elem).count();
        cout << "50%: " << stats.percentile_50_ << endl;
    }
    else
    {
        stats.percentile_50_ = NAN;
    }

    return 0;
}


