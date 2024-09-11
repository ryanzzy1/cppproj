#include <iostream>
#include <functional>

int add(int a, int b, int c, int d) {
    std::cout << "a: " << a << " b: " << b << " c: "<< c << " d: "<< d << std::endl;
    return (a + b + c) * d;
}

int main() {
    auto bound_add = std::bind(add, 5, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    std::cout << bound_add(10,15, 2) << std::endl;

    return 0;
}