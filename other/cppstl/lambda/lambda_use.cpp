// code soure:
// https://developer.aliyun.com/article/1567533?spm=a2c6h.14164896.0.0.34f847c5MRvmOe&scm=20140722.S_community@@%E6%96%87%E7%AB%A0@@1567533._.ID_1567533-RL_C-LOC_search~UND~community~UND~item-OR_ser-V_3-P0_1
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

