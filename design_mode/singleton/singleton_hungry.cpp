

#include <iostream>
#include <memory>
#include <mutex>
using namespace std;

std::mutex mtx_;
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
 * 
 */
class Singleton2
{
public:
    static Singleton2 *GetInstance()
    {
        if (m_Instance == nullptr)
        {
            std::lock_guard<std::mutex> lock(mtx_);
            if (m_Instance == nullptr){
                m_Instance = new Singleton2();
            }
        }
        return m_Instance;
    }

    static void DestroyInstance()
    {
        if (m_Instance != nullptr)
        {
            delete m_Instance;
            m_Instance = nullptr;
        }
    }

    int GetTest()
    {
        return m_Test;
    }

private:

    Singleton2(){m_Test = 20;};
    static Singleton2 *m_Instance;
    int m_Test;
    
};

Singleton2* Singleton2::m_Instance = nullptr;

class Singleton3
{
public:
    static Singleton3* GetInstance()
    {
        return const_cast<Singleton3* >(m_Instance);
    }

    static void DestroyInstance()
    {
        if (m_Instance !=nullptr)
        {
            delete m_Instance;
            m_Instance = nullptr;
        }
    }

    int GetTest()
    {
        return m_Test;
    }

private:
    Singleton3(){m_Test = 30; cout << "Singleton2 ctor called." << endl;};
    static const Singleton3 *m_Instance;
    int m_Test;
};
const Singleton3 *Singleton3::m_Instance = new Singleton3();


class Singleton4
{
public:
    static Singleton4 *GetInstance()
    {
        static Singleton4 m_Instance;
        return &m_Instance;
    }

    int GetTest()
    {
        return m_Test;
    }

private:
    Singleton4(){
        m_Test = 40;
    }
    int m_Test;
};

int main()
{
    Singleton1* s1 = Singleton1::get_instance();
    std::cout << "Singleton1 mfunction called: " << s1->GetTest() << std::endl;
    s1->DestroyInstance();

    Singleton2* s2 = Singleton2::GetInstance();
    std::cout << "Singleton2 mfunction called: " << s2->GetTest() << std::endl;
    s2->DestroyInstance();

    Singleton3 *s3 = Singleton3::GetInstance();
    std::cout << "Singleton3 mfunction called: " << s3->GetTest() << std::endl;
    Singleton3::DestroyInstance();

    Singleton4 *s4 = Singleton4::GetInstance();
    std::cout << "Singleton4 mfunction called: " << s4->GetTest() << std::endl;

    return 0;
}