#include <iostream>
// #include <iomanip>
#include <cstdint>

int main() {

    std::uint8_t a = 8;

    std::cout << "a = " << static_cast<int>(a) << std::endl;
    std::cout << "a = " << std::hex << +a << std::endl; // + 一元运算符触发整数提升
    printf("C 格式输出a = %u\n", a);// C 格式的printf 格式化输出

    return 0;
}
