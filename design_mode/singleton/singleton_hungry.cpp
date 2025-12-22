

#include <iostream>
using namespace std;

// hungry mode: 单例类定义的时候就行了实例化

class singleton{
private:
    singleton(){}
    static singleton *p;
public:
    static singleton *instance();
};

singleton *singleton::p = new singleton();
singleton* singleton::instance()
{
    return p;
}

/**
 * Singleton1 impl
 */
class Singleton1
{
public:
    static Singleton1 *get_instance(){
        if (m_Singleton1 == nullptr)
        {
            m_Singleton1 = new Singleton1();
        }
        return m_Singleton1;
    }

    static void DestroyInstance()
    {
        if(m_Singleton1 != nullptr)
        {
            delete m_Singleton1;
            m_Singleton1 = nullptr;
        }
    }

    int GetTest()
    {
        return m_test;
    }

private:
    Singleton1(){m_test = 10;};

    int m_test;

    static Singleton1* m_Singleton1;
};

Singleton1 *Singleton1::m_Singleton1 = nullptr; // static 初始化一次，可被修改；


/**
 * Singleton2 
 * 
 */
class Singleton2
{
public:

private:

};


int main()
{
    Singleton1* s1 = Singleton1::get_instance();

    std::cout << "Singleton1 mfunction called: " << s1->GetTest() << std::endl;
    s1->DestroyInstance();

    return 0;
}