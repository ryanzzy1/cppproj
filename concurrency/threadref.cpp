#include <cassert>
#include <thread>
#include <iostream>

void f(int& i) {++i; std::cout << i << std::endl;}

int main(){
    int i = 1;
    std::thread t{f, std::ref(i)};
    t.join();
    assert(i==2);
}
