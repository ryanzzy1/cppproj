#include <iostream>
#include <vector>
#include <string>
#include <sstream>

int main() {
    std::string str("This is a test.");
    std::cout << "str size: " << str.size() << "\n";
    std::vector<char> myVec(std::begin(str), std::end(str));
    
    std::cout << "myVec.data() defore:" << myVec.data() << std::endl;
    for (int i = 0; i< myVec.size(); i++)
    {
        std::cout << "myVec[" << i << "]: " << myVec[i] << std::endl;
    }

    myVec.push_back('N');
    std::stringstream ss;

    ss << "New myVec: "<< myVec.data() << "\n";

    std::cout << ss.str() << std::endl;

    std::cout << myVec.data() << std::endl;
    
    return 0;

}