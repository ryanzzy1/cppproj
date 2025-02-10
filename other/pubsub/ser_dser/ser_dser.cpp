#include <iostream>
#include <vector>
#include <string>

typedef struct alignas(4) ThroughputType
{
    uint32_t sqenum;

    uint8_t data[1];

    static const size_t overhead;

}ThroughputType;

std::vector<uint8_t> data;

int main()
{
    data.push_back('a');

    for(auto it : data)
        std::cout << it << std::endl;

}
