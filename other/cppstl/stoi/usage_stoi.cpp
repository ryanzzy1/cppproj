// stoi example
// 转自原链接：https://www.cnblogs.com/tsruixi/p/12944470.html

#include <iostream>
#include <string>

int main()
{
    std::string str_dec = "2001, A Space Odyssey";
    std::string str_hex = "40c3";
    std::string str_bin = "-100101100001";
    std::string str_auto = "0x7f";

    std::string::size_type sz;

    // int i_dec = std::stoi(str_dec, &sz);
    int i_dec = std::stoi(str_dec, nullptr,10);
    int i_hex = std::stoi(str_hex, nullptr, 16);
    int i_bin = std::stoi(str_bin, nullptr, 2);
    int i_auto = std::stoi(str_auto, nullptr, 0);

    std::cout << str_dec << ": " << i_dec << " and [" << str_dec.substr() << "]\n";
    std::cout << str_hex << ": " << i_hex << '\n';
    std::cout << str_bin << ": " << i_bin << '\n';
    std::cout << str_auto << ": " << i_auto << '\n';

    return 0;  
}
