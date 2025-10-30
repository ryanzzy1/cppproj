#include <iostream>
using namespace std;

class Component
{
public:
    virtual void operation() = 0;
};

class ConcreteComponent : public Component
{
    void operation()
    {
        cout << "None decoration ConcreteComponent." << endl;
    }
};

class Decorator : public Component
{
public:
    Decorator(Component* pComponent) : m_pComponent(pComponent){}

    void operation()
    {
        if(m_pComponent)
        {
            m_pComponent->operation();
        }
    }

protected:
    Component *m_pComponent;
};

class ConcreteDecoratorA : public Decorator
{
public:
    ConcreteDecoratorA(Component* pComponent) : Decorator(pComponent) {}

    void operation()
    {
        AddBehaviorA();
        Decorator::operation();
    }

    void AddBehaviorA()
    {
        cout << "This is ConcreteDecoratorA add behavior A." << endl;
    }

};

class ConcreteDecoratorB : public Decorator
{
public:
    ConcreteDecoratorB(Component* pComponent) : Decorator(pComponent) {}

    void operation()
    {
        AddBehaviorB();
        Decorator::operation();
    }

    void AddBehaviorB()
    {
        cout << "This is ConcreteDecoratorB add behavior A." << endl;
    }

};

int main()
{
    Component* compoenntObj = new ConcreteComponent();
    Decorator* objA = new ConcreteDecoratorA(compoenntObj);
    objA->operation();
    cout << "======================" << endl;
    Decorator* objB = new ConcreteDecoratorB(compoenntObj);
    objB->operation();
    cout << "=====================" << endl;
    Decorator* objBA = new ConcreteDecoratorB(objA);
    objBA->operation();
    cout << "=====================" << endl;

    delete objBA;
    objBA = NULL;
    delete objB;
    objB = NULL;
    delete objA;
    objA = NULL;
    delete compoenntObj;
    compoenntObj = NULL;

    return 0;
}