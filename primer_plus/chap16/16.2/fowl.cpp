// fowl.cpp -- auto_ptr a poor choice

#include <iostream>
#include <string>
#include <memory>

int main()
{
    using namespace std;
    shared_ptr<string> films[5] = 
    {
        shared_ptr<string> (new string("Fowl Balls")),
        shared_ptr<string> (new string("Duck Walks")),
        shared_ptr<string> (new string("Chicken Runs")),
        shared_ptr<string> (new string("Turkey Errors")),
        shared_ptr<string> (new string("Google Eggs"))
    };

    shared_ptr<string> pwin;
    pwin = films[4];

    cout << "The nominess for best avian baseball film are \n";
    for (int i = 0; i < 5; i++)
        cout << *films[i] << endl;
    cout << "The winner is " << *pwin << "!\n";
    cin.get();

    return 0;
}