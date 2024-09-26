#include <bitset>
#include <climits>
#include <cstring>
#include <iostream>

int main()
{
    int a[4];
    int b[10];
    std::cout << sizeof(b) << " " << sizeof(int) <<"\n";
    using bits = std::bitset<sizeof(int) * CHAR_BIT>;
    std::memset(a, 0b1111'0000'0011, sizeof a);
    for (int ai : a)
        std::cout << "sizeof(a["<< ai <<"]):" << sizeof(a[ai]) << " sizeof(a):" << sizeof(a) << "\n"<<bits(ai) << '\n';
}