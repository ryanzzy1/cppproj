/**
 * 观察者模式使用场景：
 * 
 *  1. 抽象模型的两个方面，其中一方依赖于另一方，将二者封装在独立对象中以使它们可以各自独立改变何复用；
 * 
 *  2. 当对一个对象的改变需要同时改变其他对象，而不知道具体有多少对象有待改变；
 * 
 *  3. 当一个对象必须通知其他对象，而它又不能假定其他对象是谁；即，不希望对象间是紧密耦合的
 * 
 */

 #include <iostream>
 #include <list>

using namespace std;

class Observer
{
public:
    virtual void Update(int) = 0;
};

class Subject
{
public:
    virtual void Attach(Observer *) = 0;
    virtual void Detach(Observer *) = 0;
    virtual void Notify() = 0;
};

class ConcreteObserver : public Observer
{
public:
    ConcreteObserver(Subject *pSubject) : m_pSubject(pSubject){
        cout << "ConcreteObserver ctor." << endl;
    }

    void Update(int value)
    {
        cout << "ConcreteObserver get the update. New State:" << value << endl;
    }

private:
    Subject *m_pSubject;
};

class ConcreteObserver2 : public Observer
{
public:
    ConcreteObserver2(Subject *pSubject) : m_pSubject(pSubject){
        cout << "ConcreteObserfver2 ctor." << endl;
    }

    void Update(int value)
    {
        cout << "ConcreteObserver2 get the update. New State:" << value << endl;
    }

private:
    Subject *m_pSubject;
};

class ConcreteSubject : public Subject
{
public:
    void Attach(Observer *pObserver) override;
    void Detach(Observer *pObserver) override;
    void Notify() override;

    void SetState(int state)
    {
        m_iState = state;
    }

private:
    std::list<Observer *> m_oberserverList;
    int m_iState;
};

void ConcreteSubject::Attach(Observer *pObserver)
{
    m_oberserverList.push_back(pObserver);
}

void ConcreteSubject::Detach(Observer *pObserver)
{
    m_oberserverList.remove(pObserver);
}

void ConcreteSubject::Notify()
{
    std::list<Observer *>::iterator it = m_oberserverList.begin();
    while (it != m_oberserverList.end())
    {
        (*it)->Update(m_iState);
        ++it;
    }
}

int main()
{
    // Create subject
    ConcreteSubject *pSubject = new ConcreteSubject();

    // create Observer
    Observer *pObserver = new ConcreteObserver(pSubject);
    Observer *pObserver2 = new ConcreteObserver2(pSubject);

    // change the state
    pSubject->SetState(10);

    // Register the observer
    pSubject->Attach(pObserver);
    pSubject->Attach(pObserver2);

    pSubject->Notify();

    // unregister the observer 
    pSubject->Detach(pObserver);
    
    pSubject->SetState(20);
    pSubject->Notify();

    delete pObserver;
    delete pObserver2;
    delete pSubject;

}