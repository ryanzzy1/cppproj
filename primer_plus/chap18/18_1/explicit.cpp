#include <iostream>

class Plebe
{
public:
    Plebe(int n) { std::cout << "Plebe(int) called.\n parameter = " << n << std::endl;  }
    explicit Plebe(double m) {std::cout << "Plebe(double) called.\n parameter = " << m << std::endl; }
};


int main()
{
    // Plebe a(), b;
    Plebe a = 10;
    Plebe b = 3.1;
    b = Plebe(3.2);

    return 0;
}