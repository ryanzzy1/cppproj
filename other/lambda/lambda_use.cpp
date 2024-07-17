#include <iostream>
#include <type_traits>

void demo() {

    auto safeAdd = [](auto a, auto b) -> decltype(a+b) {
        static_assert(std::is_arithmetic<decltype(a)>::value && 
                        std::is_arithmetic<decltype(b)>::value,
                        "Only artithmetic types are supported");
        return a + b;
    };

    std::cout << safeAdd(1,2) << std::endl;
}

int main()
{
    demo();
    return 0;
}