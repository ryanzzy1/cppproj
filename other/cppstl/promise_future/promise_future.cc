#include <iostream>
#include <chrono>
#include <vector>
#include <future>
#include <numeric>
#include <thread>

using namespace std;
using namespace chrono;

void func1(vector<int>& vec, promise<int> pr) {
    int res = accumulate(vec.begin(), vec.end(), 0);
    pr.set_value(res);
}

vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

int main()
{
    promise<int> pr;
    future<int> ft = pr.get_future();
    thread t1(func1, ref(vec), move(pr));
    int res = ft.get();
    cout << "res = " << res << endl;

    return 0;
}