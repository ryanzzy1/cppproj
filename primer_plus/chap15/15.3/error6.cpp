// newexcp.cpp -- the bad_alloc exception

#include <iostream>
#include <new>
#include <cstdlib>  // for exit(), EXIT_FAILURE
using namespace std;

struct Big
{
    // double stuff[20000];
    double stuff[2000000000];
};

int main()
{
    Big * pb;
    try {
        cout << "Try to get a big block of memory:\n";
        // pb = new Big[10000];    //1600000000 bytes
        // pb = new Big[100000];    //exception create
        pb = new (std::nothrow) Big[10000]; // c++ stlib usage 
        if (pb == 0) {
            cout << "Could not allocate memory.\n";
            exit(EXIT_FAILURE);
        }
        cout << "Got past the new request: \n";
    }
    catch (bad_alloc &ba)
    {
        cout << "Caught the exception!\n";
        cout << ba.what() << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Memory successfully allocated\n";
    pb[0].stuff[0] = 4;
    cout << pb[0].stuff[0] << endl;
    delete [] pb;
    return 0;
}

