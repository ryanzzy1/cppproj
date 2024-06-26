// usebrass2.cpp -- polymorphic example
// compile with brass.cpp

#include <iostream>
#include <string>
#include "brass.h"
const int CLIENTS = 4;

int main()
{
    using std::cin;
    using std::cout;
    using std::endl;

    Brass * p_clients[CLIENTS];
    std::string temp;
    long tempnum;
    double tempbal;
    char kind;

    for (int i = 0; i < CLIENTS; i++)
    {
        cout << "Enter client's name: ";
        getline(cin,temp);
        cout << "Enter client's account number: ";
        cin >> tempnum;
        cout << "Enter opening balance: $";
        cin >> tempbal;
        cout << "Enter 1 for Brass Account or "
             << "2 for BrassPlus Account: ";
        while (cin >> kind && (kind != '1' && kind != '2'))
            cout << "Enter either 1 or 2: ";
        if (kind == '1')
            p_clients[i] = new Brass(temp, tempnum, tempbal);
        else
        {
            double tmax, trate;
            cout << "Enter the overdraft limit: $";
            cin >> tmax;
            cout << "Enter the interest rate "
                 << "as a decimal fraction: ";
            cin >> trate;
            p_clients[i] = new BrassPlus(temp, tempnum, tempbal,
                                         tmax, trate);
        }
        while (cin.get() != '\n')
            continue;
    }
    cout << endl;
    // polymorphism code 
    // if array element point to Brass object, call the Brass::ViewAcct()
    // if array element point to BrassPlus object, call the BrassPlus::ViewAcct()
    // if Brass::ViewAcct() declared to virtual, Brass::ViewAcct() will be called in any cases.

    for ( int i = 0; i < CLIENTS; i++)
    {
        p_clients[i]->ViewAcct();
        cout << endl;
    }

    //  如果析构函数不是虚的，则将只调用对应于指针类型的析构函数。虚析构函数则会调用相应对象类型的析构函数。如果指向的是BrassPlus的析构函数
    //  然后自动调用基类的析构函数。 因此， 使用虚析构函数可以确保正确的析构函数序列被调用。
    for (int i = 0; i < CLIENTS; i++)
    {
        delete p_clients[i];  //free memory
    }
    cout << "Done.\n";

    return 0;
}