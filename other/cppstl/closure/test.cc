#include <iostream>
#include <complex>
#include <cmath>

class MyLambda {
public:
    int n;
    double operator()(int a) const{
        return std::pow(a, n);
    }

    double sum(int a, int b, double(*term)(int)) {
        return a > b ? 0.0 : term(a) + sum(a + 1, b, term);
    }
};

int main() {
    int n;
    std::cin >> n;
    MyLambda x{n};
    sum(1, 5, x);
}