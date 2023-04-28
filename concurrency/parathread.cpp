#include <iostream>
#include <thread>

void f(int i = 1) {std::cout << i << std::endl;}

int main(){
    std::thread t{f, 42};  // std::thread t{f} error, will igore the argument i
    t.join();
}
