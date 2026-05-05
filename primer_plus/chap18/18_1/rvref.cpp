// rvref.cpp
#include <iostream>

inline double f(double tf) {return 5.0 * (tf - 32.0)/9.0;};

char * ptr = "hello World";

int main()
{
    using namespace std;
    cout << "char * ptr 中ptr输出：\nptr = " << ptr
         << "\n&ptr 输出：" << &ptr
         << left << "\n指针地址(void*)ptr = " << (void*)ptr 
         << "\n*ptr = " << *ptr << endl;

    double tc = 21.5;
    double && rd1 = 7.07;
    double && rd2 = 1.8 * tc + 32.0;
    double && rd3 = f(rd2);
    cout << "tc value and address: " << tc << ", " << &tc << endl;
    cout << "rd1 value and address: " << rd1 << ", " << &rd1 << endl;
    cout << "rd2 value and address: " << rd2 << ", " << &rd2 << endl;
    cout << "rd3 value and address: " << rd3 << ", " << &rd3 << endl;
    cin.get();
    return 0;
}
