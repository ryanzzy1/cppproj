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
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>

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

class Singleton1
{
private:
    static Singleton1 *pinstance_;
    static std::mutex mutex_;

protected:
    Singleton1(const std::string value) : value_(value){}
    ~Singleton1(){}
    std::string value_;

public:
    Singleton1(Singleton1 &other) = delete;
    void operator=(const Singleton1 &) = delete;
    static Singleton1 *GetInstance(const std::string& value);
    void SomeBusinessLogic()
    {

    }
    std::string value() const {
        return value_;
    }
};

Singleton1* Singleton1::pinstance_{nullptr};
std::mutex Singleton1::mutex_;

Singleton1 *Singleton1::GetInstance(const std::string& value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (pinstance_ == nullptr)
    {
        pinstance_ = new Singleton1(value);
    }
    return pinstance_;
}

void ThreadFoo()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    Singleton1 *singleton = Singleton1::GetInstance("FOO");
    cout << singleton->value() << endl;
}

void ThreadBar()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    Singleton1 *singleton = Singleton1::GetInstance("BAR");
    std::cout << singleton->value() << endl;
}

int main()
{

    singleton *s1 = singleton::instance();
    singleton *s2 = singleton::instance();
    std::thread t1(ThreadFoo);
    std::thread t2(ThreadBar);
    /*
    int test[10] = {10,11,12,13,14,15,16,17,18,19};
    std::vector<int> vec;
    for(int i = 0; i < 10; i++){
        cout << "test[" << i << "]%10 = " << test[i] % 10 << endl;

        vec.push_back(test[i]%10);
    }
    for (auto it = vec.begin(); it != vec.end(); ++it){
        cout << "vec[" << *it <<"] = " << *it << endl;
    }*/

    t1.join();
    t2.join();

    return 0;
}