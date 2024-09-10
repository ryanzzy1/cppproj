#include <iostream>
#include <functional>

void great() {
    std::cout << "Hello World!" << std::endl;
}

int sum(int a, int b) {
    std::cout << a + b << std::endl;
    return 0;
}

int main() {
    std::function<void()> f = great;    // 使用函数
    f();

    std::function<void()> lambda = []() {
        std::cout << "Hello, Lambda!" << std::endl;
    };
    lambda();

    std::function<int(int, int)> fx = sum;
    fx(2,3);

    return 0;
}
