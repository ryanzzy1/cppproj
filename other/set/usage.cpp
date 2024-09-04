#include <iostream>
#include <set>

int main()
{
    std::set<int> mySet;

    // insert element
    mySet.insert(10);
    mySet.insert(20);
    mySet.insert(30);
    mySet.insert(40);

    // output element in set
    std::cout << "Set contains: ";
    for (int num : mySet) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    // lookup element
    if (mySet.find(20) != mySet.end()) {
        std::cout << "20 is in the set." << std::endl;
    } else {
        std::cout << "20 is not in the set." << std::endl;
    }

    // delete element
    mySet.erase(20);
    std::cout << "Erase 20 in set." << std::endl;
    for (int i : mySet) {
        std::cout << i << std::endl;
    }

    // check if set is empty
    if (mySet.empty()) {
        std::cout << "The set is empty." << std::endl;
    } else {
        std::cout << "The set is not empty." << std::endl;
    }

    // output element num in set
    std::cout << "The set contains " << mySet.size() << "elements." << std::endl;

    return 0;
}