#include "tstack.h"
#include <string>
#include <iostream>
#include <cctype>

using std::cin;
using std::cout;
using std::endl;
using std::string;

int main()
{
    Stack<int> kernels;
    Stack<string> colonels;
    char ch;
    int po;
    string name;
    cout << "Please enter A to add a purchase order,\n"
         << "P to process a PO, or Q to quit.\n";
    while (cin >> ch && toupper(ch) != 'Q')
    {
        while(cin.get() != '\n')
            continue;
        if (!isalpha(ch))
        {
            cout << '\a';
            continue;
        }
        switch(ch)
        {
            case 'A':
            case 'a':   cout << "Enter a PO number to add: ";
                        cin >> po;
                        if (kernels.isfull())
                            cout << "Stack already full\n";
                        else
                            kernels.push(po);
                        cout << "Enter a name to add: ";
                        cin >> name;
                        if (colonels.isfull())
                            cout << "Stack already full\n";
                        else
                            colonels.push(name);
                        break;
            case 'P':
            case 'p':   if (kernels.isempty())
                            cout << "Stack already empty\n";
                        else
                        {
                            kernels.pop(po);
                            cout << "PO #" << po << " popped\n";
                        }
                        if (colonels.isempty())
                            cout << "Stack already empty\n";
                        else
                        {
                            colonels.pop(name);
                            cout << name << " popped\n";
                        }
                        break;
        }       
        cout << "Please enter A to add a purchase order,\n"
             << "P to process a PO, or Q to quit.\n";
        
    }
    cout << "Bye\n";

    return 0;
}