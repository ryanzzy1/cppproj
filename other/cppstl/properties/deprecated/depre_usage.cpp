#include <iostream>

[[deprecated]]
void TriassicPeriod() {
    std::clog << "Triassic Period: [251.9 - 208.5] million years ago.\n";
}

[[deprecated("Use NeogenePeriod() instead.")]]
void JurassicPeriod() {
    std::clog << "Jurassic Period: [201.3 -152.1] million years ago.\n";
}

[[deprecated("Use calcSomethingDifferently(int).")]]
int calcSomething(int x) {
    return x * 2;
}

int main()
{
    TriassicPeriod();
    JurassicPeriod();
    std::cout << "calcResult: " << calcSomething(3) << "\n";
}