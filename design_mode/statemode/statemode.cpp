#include <iostream>
using namespace std;

class Context;

class State
{
public:
    virtual void Handle(Context *pContext) = 0;
};

class ConcreteStateA : public State
{
public:
    virtual void Handle(Context *pContext)
    {
        cout << "This is ConcreteStateA handling." << endl; 
    }
};

class ConcreteStateB : public State
{
public:
    virtual void Handle(Context *pContext)
    {
        cout << "This is ConcreteStateB Handling." << endl;
    }
};

class Context
{
public:
    Context(State *pState) : m_pState(pState){
        cout << "Context ctor called." << endl;
    }

    void Request(){
        if(m_pState)
        {
            m_pState->Handle(this);
        }
    }

    void changeState(State *pState){
        m_pState = pState;
    }
private:
    State *m_pState;
};

int main()
{
    State *pStateA = new ConcreteStateA();
    State *pStateB = new ConcreteStateB();
    Context *pContext = new Context(pStateA);
    pContext->Request();

    pContext->changeState(pStateB);
    pContext->Request();

    delete pContext;
    delete pStateB;
    delete pStateA;
}