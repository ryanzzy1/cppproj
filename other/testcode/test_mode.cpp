#include <iostream>

using namespace std;

int main()
{
    for (int i = 9; i >= 0; i--){
        cout << i <<" % 256 = " << (i % 256) << endl;
    }

    int m = 256;
    for (int j = 0; j <= 9; j++) {
        cout << j << " * " << m << " % 256 = " << j * m % 256 << endl;
        cout << j << " * " << m << " / 256 = " << j * m / 256 << endl;
        cout << "-----" << endl;
    }

    return 0;
}