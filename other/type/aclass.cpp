// refer to : https://blog.csdn.net/qq_38158479/article/details/122334765
// #include <iostream>
// using namespace std;
// template <typename T, bool val>
// struct AClass
// {
//     AClass() //构造函数
//     {
//         cout << "AClass::AClass()执行了" << endl;
//         if (val)
//         {
//             T tmpa = 15;
//         }
//         else
//         {
//             T tmpa = "abc"; //这里有问题
//         }
//     }
// };
// int main()
// {
//     AClass<int, true> obj;
// 	return 0;
// }

// #include <iostream>
// using namespace std;
// template <typename T, bool val>
// struct AClass
// {
//     AClass() //构造函数
//     {
//         cout << "AClass::AClass()执行了" << endl;
//         if constexpr (val)
//         {
//             T tmpa = 15;
//         }
//         else
//         {
//             T tmpa = "abc";
//         }
//     }
// };
// int main()
// {
//     AClass<int, true> obj;
// 	return 0;
// }

#include <iostream>
#include <ratio>
using namespace std;
template <typename T, bool val>
struct AClass
{
    AClass() //构造函数
    {
        cout << "AClass::AClass()执行了" << endl;
        fun(bool_constant<val>());
    }
    void fun(std::true_type)
    {
        T tmpa = 15;
    }
    void fun(std::false_type)
    {
        T tmpa = "abc";
    }
};
int main()
{
    AClass<int, true> obj;
	return 0;
}
