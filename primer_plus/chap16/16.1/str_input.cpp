#include <iostream>
#include <string>
#include <thread>
#include <chrono>
using namespace std;


int main()
{
    // char info[100];
    // cin >> info;
    // // cout << "cin info: " << info << endl;
    // cin.getline(info, 100, ':'); // read up to :, discard :
    // cout << "getline info: " << info << endl;
    // cin.get(info, 100);
    // cout << "get info: " << info << endl;


    string stuff;
    // cin >> stuff;
    cout << "cin string stuff: " << stuff << endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    getline(cin, stuff, ':');   // read up to :, discard :
    cout << "getline cin string stuff: " << stuff << endl;
}