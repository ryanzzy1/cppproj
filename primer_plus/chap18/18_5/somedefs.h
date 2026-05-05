// somedefs.h
#include <iostream>
#include <functional>

template <typename T, typename F>
T use_f(T v, F f)
{
    static int count = 0;
    count++;
    std::cout << " use_f count = " << count
              << ", &count = " << &count << std::endl;
    return f(v);
}

// simplify 2, 修改函数模板，将第二个参数声明为function的包装器对象
/*
template <typename T>
T use_f(T v, std::function<T(T)> f) // f的调用特征标为T(T)
{
    static int count = 0;
    count++;
    std::cout << " use_f count = " << count
              << ", &count = " << &count << std::endl;
    return f(v);
}*/

class Fp
{
private:
    double z_;
public:
    Fp(double z = 1.0) : z_(z) {}
    double operator() (double p) {return z_ * p;}
};

class Fq
{
private:
    double z_;
public:
    Fq(double z = 1.0) : z_(z) {}
    double operator() (double q) {return z_ + q;}
};