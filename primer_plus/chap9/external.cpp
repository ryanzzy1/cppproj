// external.cpp
#include <iostream>
using namespace std;

// external variable
double warming = 0.3;

// function prototype
void update(double dt);
void local();

int main()
{
    cout << "Global warming: " << warming << " degrees\n";

    update(0.1);
    cout << "Global warming: " << warming << " degrees\n";

    local();
    cout << "Global warming: " << warming << " degrees\n";

    return 0;
}