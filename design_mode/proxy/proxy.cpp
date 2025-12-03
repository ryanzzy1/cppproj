/**
 *  远程代理、虚代理、保护代理、智能引用
 * 
 *  远程代理：为一个对象在不同的地址空间提供局部代理；
 * 
 *    虚代理：根据需求创建开销很大的对象；
 * 
 *   保护代理：控制对原始对象的访问权限，用于对象应该有不同访问权限的时候；
 * 
 *   智能引用：在访问对象时执行附加操作。典型用途：1）指向实际对象的引用计数，没对象引用时，可以自动释放对象；2）线程锁定；3）检查对象是否已经被锁定等。
 * 
 **/
#include <iostream>

using namespace std;

#define SAFE_DELETE(p) if(p){delete p; cout << "SAFE_DELETE CALLED" << endl; p = NULL;}

class CSubject
{
public:
    CSubject(){
        cout << "CSubject Constructor called." << endl;

    }
    virtual ~CSubject(){
        cout << "CSubject Destructor called." << endl;
    }

    virtual void Request() = 0;
};

class CRealSubject : public CSubject
{
public:
    CRealSubject(){
        cout << "CRealSubject Constructor called." << endl;
    }
    ~CRealSubject(){
        cout << "CRealSubject Destructor called." << endl;
    }

    void Request() override
    {
        cout << "CRealSubject Request called." << endl;
    }
};

class CProxy : public CSubject
{
public:
    CProxy() : m_pRealSubject(NULL){
        cout << "CProxy Constructor called." << endl;
    }
    ~CProxy()
    {
        SAFE_DELETE(m_pRealSubject);
        cout << "CProxy Destructor called." << endl;
    }

    void Request()
    {
        if (NULL == m_pRealSubject)
        {
            m_pRealSubject = new CRealSubject();
        }
        cout << "CProxy Rquest called." << endl;
        m_pRealSubject->Request(); // 结束后析构指针指向的派生类，其次基类， 其次this对象CProxy析构
    }

private:
    CRealSubject* m_pRealSubject;
};

int main()
{
    CSubject *pSubject = new CProxy();
    pSubject->Request();
    SAFE_DELETE(pSubject);
}
