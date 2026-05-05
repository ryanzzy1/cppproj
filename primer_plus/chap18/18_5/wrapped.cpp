// wrapped.cpp -- using a function wrapper as an argument

#include "somedefs2.h"
#include <iostream>
#include <functional>

double dub(double x) {return 2.0*x;}
double square(double x) {return x*x;}

int main()
{
    using std::cout;
    using std::endl;
    using std::function;

    double y = 1.21;
    /*original method*/
    /*
    function<double(double)> ef1 = dub;
    function<double(double)> ef2 = square;
    function<double(double)> ef3 = Fp(10.0);
    function<double(double)> ef4 = Fq(10.0);
    function<double(double)> ef5 = [](double u) {return u*u;};
    function<double(double)> ef6 = [](double u) {return u+u/2.0;};
    */

    // simplify 1
    typedef function<double(double)> fdd;
    // 在调用中创建并初始化对象为指定实参

    // simplify 2, 修改函数模板，将第二个参数声明为function的包装器对象

    cout << "Funtion pointer dub:\n";
    // cout << " " << use_f(y, ef1) << endl;
    // cout << " " << use_f(y, fdd(dub)) << endl;
    cout << " " << use_f<double>(y, dub) << endl;   // 函数模板的具体化

    cout << "Funtion pointer square:\n";
    // cout << " " << use_f(y, ef2) << endl;
    // cout << " " << use_f(y, fdd(square)) << endl;
    cout << " " << use_f<double>(y, square) << endl;


    cout << "Funtion object Fp:\n";
    // cout << " " << use_f(y, ef3) << endl;
    // cout << " " << use_f(y, fdd(Fp(10.0))) << endl;
    cout << " " << use_f<double>(y, Fp(10.0)) << endl;


    cout << "Funtion object Fq:\n";
    // cout << " " << use_f(y, ef4) << endl;
    // cout << " " << use_f(y, fdd(Fq(10.0))) << endl;
    cout << " " << use_f<double>(y, Fq(10.0)) << endl;


    cout << "Lambda expression 1:\n";
    // cout << " " << use_f(y, ef5) << endl;
    // cout << " " << use_f(y, fdd([](double u) {return u*u;})) << endl;
    cout << " " << use_f<double>(y, [](double u) {return u*u;}) << endl;

    cout << "Lambda expression 2:\n";
    // cout << " " << use_f(y, ef6) << endl;
    // cout << " " << use_f(y, fdd([](double u) {return u+u/2.0;})) << endl;
    cout << " " << use_f<double>(y, [](double u) {return u+u/2.0;}) << endl;

    return 0;
}