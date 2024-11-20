
// 转自： https://www.cnblogs.com/mangoyuan/p/6446046.html
#include <iostream>
using namespace std;


// implement specific tag
struct A
{
    /* data */
};

struct B : A { };   


/* unknown_class structure*/
template <class AorB>
struct unknown_class {
    typedef AorB return_type;
};


// traits 偏特化版本
/* 特性萃取器*/
template <class unknown_class>
struct unknown_class_traits {
    typedef typename unknown_class::return_type return_type;
};

/* 特性萃取器-- 针对原生指针*/
template <class T>
struct unknown_class_traits<T*> {
    typedef T return_type;
};

/* 特性萃取器-- 针对指向常数*/
template <class T>
struct unknown_class_traits<const T*> {
    typedef const T return_type;
};


/*决定使用哪一个类型*/
// return_type() 构造函数
template <class unknown_class>
inline typename unknown_class_traits<unknown_class>::return_type
return_type(unknown_class) {
    typedef typename unknown_class_traits<unknown_class>::return_type RT;
    return RT();
}


/* func 各版本重载*/

template <class unknown_class>
inline typename unknown_class_traits<unknown_class>::return_type
__func(unknown_class, A) {
    cout << "use A flag" << endl;
    return A();
}

template <class unknown_class>
inline typename unknown_class_traits<unknown_class>::return_type
__func(unknown_class, B) {
    cout << "use B flag" << endl;
    return B();
}

template <class unknown_class, class T>
T
__func(unknown_class,  T) {
    cout << "use origin ptr" << endl;
    return T();
}


template <class unknown_class>
inline typename unknown_class_traits<unknown_class>::return_type    // 萃取器获得对应型别
func(unknown_class u) {
    typedef typename unknown_class_traits<unknown_class>::return_type return_type;
    return __func(u, return_type());    // 调用return_type() 构造函数 当tag
}

int main() {
    unknown_class<B> b;
    unknown_class<A> a;
    // unknown_class<int> i;

    int value = 1;
    int *p = &value;

    A v1 = func(a);
    B v2 = func(b);

    int v3 = func(p);

    char ch = getchar();
}