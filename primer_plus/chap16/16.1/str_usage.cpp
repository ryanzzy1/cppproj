// str1.cpp -- introducing the string class

#include <iostream>
#include <string>

int main()
{
    using namespace std;
    string one("Lottery Winner!");      // ctor string(const * s)
    cout << one << endl;                // overloaded <<
    string two(10, '$');                // ctor string(size_type n, char c)
    cout << two << endl;                
    string three(one);                  // ctor string(const string &str)
    cout << three << endl;
    one += " Oops!";                    // overloaded +=
    cout << one << endl;
    two = "Sorry! That was ";
    three[0] = 'P';
    string four;                        // ctor string()
    four = two + three;                 // overloaded +, =
    cout << four << endl;
    char alls[] = "All's well that ends well";
    string five(alls, 20);              // ctor string(const char * s, size_type n)
    cout << five << "!\n";
    string six(alls + 6, alls + 10);    // template<class Iter> string(Iter begin, Iter end)
    cout << six << ", ";
    string seven(&five[6], &five[10]);  // template<class Iter> string(Iter begin, Iter end)
    cout << seven << "...\n";
    string eight(four, 7, 16);          // string(const string &str, string size_type pos = 0, size_type n = npos)
    cout << eight << " in motion!" << endl;

    return 0;
}