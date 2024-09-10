#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

bool compare(int a, int b) {
    return a < b;
}

int main() {
    std::vector<int> v = {5, 3, 9, 1, 4};
    std::sort(v.begin(), v.end(), compare); // 使用自定义比较函数
    for (int i : v) {
        std::cout << i << " ";
    }

    std::sort(v.begin(), v.end(), std::less<int>()); //stl compare function
    for (int i : v) {
        std::cout << i << " ";
    }

    return 0;
}