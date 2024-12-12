#include <iostream>

typedef struct alignas(4) ThroughputType
{
    // identifies the sample sent
    uint32_t seqnum;
    // actual payload
    uint8_t data[1];
    // This struct overhead
    static const size_t overhead;

} ThroughputType;

uint32_t count = 1;

int main()
{
    std::cout << "count: " << count <<  std::endl;
    std::cout << "sizeof(decltype(ThroughputType::seqnum)): " << sizeof(decltype(ThroughputType::seqnum)) << std::endl;
    std::cout << "~3: " << (~3) << "\n(count + 3) & ~3 : " << ((count + 3) & ~3) << std::endl;
    count = sizeof(decltype(ThroughputType::seqnum)) + ((count + 3) & ~3);

    std::cout << "count: " << count <<  std::endl;

    std::cout << "(( + 3) & ~3): " << ((5 + 3) & ~3) << std::endl;
}