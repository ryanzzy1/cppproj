#include <iostream>

void swapr(int & a, int & b);
void swapp(int * a, int * b);
void swapv(int a, int b);

int main()
{
    using namespace std;

    int wallet1 = 300;
    int wallet2 = 350;
    cout << "wallet1 = " << wallet1 << " wallet2 = " << wallet2 << endl;

    cout << "using reference " << endl;
    swapr(wallet1, wallet2);
    cout << "wallet1 = " << wallet1 << " wallet2 = " << wallet2 << endl;

    cout << "using pointer " << endl;
    swapp(&wallet1, &wallet2);
    cout << "wallet1 = " << wallet1 << " wallet2 = " << wallet2 << endl;
    
    cout << "using value " << endl;
    swapv(wallet1, wallet2);
    cout << "wallet1 = " << wallet1 << " wallet2 = " << wallet2 << endl;

    return 0;
}

void swapr(int & a, int & b)
{
    int temp;

    temp = a;
    a = b;
    b = temp;
}

void swapp(int * a, int * b)
{
    int temp;

    temp = *a;
    *a = *b;
    *b = temp;
}

void swapv(int a, int b)
{
    int temp;
    temp = a;
    a = b;
    b = temp;
    std::cout << "a = " << a << " b = " << b << std::endl;

}