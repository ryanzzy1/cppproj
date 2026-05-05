// usedma.cpp
#include "dma.h"
#include <iostream>

int main()
{
    using std::cout;
    using std::endl;

    baseDMA shirt("Portablly", 8);
    lacksDMA balloon("red", "Blimpo", 4);
    hasDMA map("Mercator", "Buffalo", 5);

    cout << "Displaying baseDMA object:\n";
    cout << shirt;
    cout << "Displaying lacksDMA object:\n";
    cout << balloon;
    cout << "Displaying hasDMA object:\n";
    cout << map;

    lacksDMA balloon2(balloon);
    cout << "Displaying copy of balloon:\n";
    cout << balloon2;
    hasDMA map2;
    map2 = map;
    cout << "Displaying copy of map:\n";
    cout << map2;

    return 0;
}