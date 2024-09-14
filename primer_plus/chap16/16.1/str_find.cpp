#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using std::string;
using std::cin;
using std::cout;



int main()
{
    string test = {"The Apple is a good try!\n"};
    int loc;
    char letter;
    cin >> letter;
    loc = test.find(letter);
    cout << letter << " 's loc is " << loc << " in test\n";

}