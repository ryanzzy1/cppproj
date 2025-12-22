/**
 * @file singleton.cpp  
 * 
 * @brief singleton implementation
 * 
 * @ref light-city.github.io/stories_things/
 *  
 * @note details refer to the github address above. 
*/

/*  cstr 声明为private或protected 防止被外部函数实例化， 内部有一个静态类指针保存唯一实例
 *  实例的实现通过一个公有的方法来实现，这个方法返回类的唯一实例。
 * 
 */

// 单线程模式下可用，但多线程时非线程安全
// lazymode: 第一次用到类的实例的时候才会去实例化

#include <iostream>

using namespace std;

class singleton {
private:
    singleton(){}
    static singleton *p;
public:
    static singleton *instance();
};

singleton *singleton::p = nullptr;

singleton* singleton::instance(){
    if (p == nullptr)
        p = new singleton();
    return p;
}