#include <iostream>

using namespace std;

int main (int argc, char* argv[])
{
    if ( argc == 0)
        cout << "no argc showing in this condition.\n" 
             << "argv:" << argv[argc] << endl;

    else if ( argc == 1) 
        cout << "argc shows only one parameter.\n" 
        << "argv[0]:" << argv[0] << endl;
    else 
        cout << "argc shows more than one parameter.\n";
        for ( int i = 0; i < argc; i++)
            cout << "argv[" << i << "]:" << argv[i] << endl;

    char* test = nullptr;
    char test1[] = {'a','b','c'}; 
    char* b[] = {nullptr};
    cout << "char* test, test address:" << &test << "\n";
    if (test == nullptr)
        // cout << "(*test) value: " << (*test) << endl;
    // else
        cout << "can not abtain (*test) value for a nullptr." << endl;

    cout << "char test1[], sizeof(test1): " << sizeof(test1) << "\n&test1:" << &test1 << "\ntest1:" << test1
         << "\ntest1[0]: " << &test1[0] << "\ntest1[0]:" << test1[0] << "\n";

    cout << "cahr* b[], b[] address: " << b << " &b[0] address:"<< &b[0] << endl;

    return 0;
}