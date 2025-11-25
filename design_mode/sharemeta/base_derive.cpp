#include <iostream>
#include <map>
#include <vector>

using namespace std;

// meta
class A 
{
public:
    A( int a) : x(a){
        std::cout << "This is A ctor." << std::endl;
    }

    void setNum(int count)
    {
        x = count;
    }

    int getNum(){
        return x;
    }

private:
    int x;
};

class B : public A
{
public:
    B(int b) : A(b)
    {
        std::cout << "This is B ctor." << std::endl;
    }
};

// derived meta 
class AFactory
{
public:
    A* GetA(int num)
    {
        A* pA = nullptr;

        if (m_vecA.empty())
        {
            pA = new B(num);
            m_vecA.push_back(pA);
        } else {
            for(vector<A*>::iterator it = m_vecA.begin(); it != m_vecA.end(); ++it)
            {
                if((*it)->getNum() == num)
                {
                    pA = *it;
                    break;
                }
            }

            if (pA == NULL)
            {
                pA = new B(num);
                m_vecA.push_back(pA);
            }
        }
        return pA;
    }

    ~AFactory()
    {
        for(vector<A*>::iterator it = m_vecA.begin(); it != m_vecA.end(); ++it)
        {
            if(*it != NULL)
            {
                delete *it;
                *it = NULL;
            }
        }
    }

private:
    vector<A*> m_vecA;
};

class AUsage
{
public:
    void setA(A *pA)
    {
        if(pA->getNum())
        {
            cout << "A already get a num " << pA->getNum() << endl;
        }

        m_mapA.insert(pair<int, A*>(pA->getNum(), pA));
    }

    void showAllA()
    {
        for(map<int, A*>::iterator it = m_mapA.begin(); it != m_mapA.end(); ++it)
        {
            cout << "A num: " << it->first << endl;
        }
    }

private:
    map<int, A*> m_mapA;
};


int main()
{
    // B b(7);

    AFactory *apFactory = new AFactory();
    AUsage *apUsage = new AUsage();

    // 1st
    A *pA1 = apFactory->GetA(5);
    pA1->setNum(5);
    apUsage->setA(pA1);

    // 2nd 
    A *pA2 = apFactory->GetA(6);
    pA2->setNum(6);
    apUsage->setA(pA2);

    // 3rd
    A *pA3 = apFactory->GetA(7);
    pA3->setNum(7);
    apUsage->setA(pA3);

    // 4th
    A *pA4 = apFactory->GetA(8);
    pA4->setNum(8);
    apUsage->setA(pA4);

    cout << "Show all A:" << endl;
    apUsage->showAllA();

    if(apUsage != NULL)
    {
        delete apUsage;
        apUsage = NULL;
    }
    if(apFactory != NULL)
    {
        delete apFactory;
        apFactory = NULL;
    }

    return 0;
}