/**
 * 使用场合：
 * 
 *  1. 需要保存和恢复对象的状态时，例如在实现撤销/重做功能时。
 *  2. 需要在不暴露对象内部结构的情况下保存对象状态
 */

#include <iostream>
#include <cwchar>
using namespace std;

struct State
{
    wchar_t wcsState[260];
};

// 备忘录类
class Memento
{
public:
    Memento(State *pState) : m_pState(pState) {cout << "Memento ctor called." << endl;}

    State *GetState() {
        return m_pState;
    }

private:
    friend class Originator;
    
    State *m_pState;
};

// 原发器类
class Originator
{
public:
    Originator() : m_pState(nullptr) {cout << "Originator ctor called." << endl;}

    ~Originator()
    {
        if (m_pState)
        {
            delete m_pState;
            m_pState = nullptr;
        }
    }

    void SetMemento(Memento *pMemento);

    Memento *CreateMemento();

    void SetValue(const wchar_t *value)
    {
        // clear buffer and copy safely (ensure null-termination)
        std::wmemset(wcsValue, 0, 260);
        std::wcsncpy(wcsValue, value, 259);
        wcsValue[259] = L'\0';
    }

    void PrintSate(){
        wcout << "Originator PrintState: " << wcsValue << endl;
    }

private:
    State *m_pState;

    wchar_t wcsValue[260];
};

Memento *Originator::CreateMemento()
{
    m_pState = new State;
    if (m_pState == nullptr)
    {
        return nullptr;
    }

    Memento *pMemento = new Memento(m_pState);

    wcsncpy(m_pState->wcsState, wcsValue, 259);

    return pMemento;
}

void Originator::SetMemento(Memento *pMemento)
{
    m_pState = pMemento->GetState();

    wmemset(wcsValue, 0, 260);
    wcsncpy(wcsValue, m_pState->wcsState, 259);
}

class Caretaker
{
public:
    Memento *GetMemento(){
        return m_pMemento;
    }

    void SetMemento(Memento *pMemento){
        if (m_pMemento)
        {
            delete m_pMemento;
            m_pMemento = nullptr;
        }

        m_pMemento = pMemento;
    }

private:
    Memento *m_pMemento;
};

int main()
{
    // originator 
    Originator *pOriginator = new Originator();
    pOriginator->SetValue(L"on");
    pOriginator->PrintSate();

    // caretaker and memento creation
    Caretaker *pCaretaker = new Caretaker();
    pCaretaker->SetMemento(pOriginator->CreateMemento());

    // set new state
    pOriginator->SetValue(L"off");
    pOriginator->PrintSate();

    // recover to old state
    pOriginator->SetMemento(pCaretaker->GetMemento());
    pOriginator->PrintSate();

    if (pCaretaker)
    {
        delete pCaretaker;
    }

    if (pOriginator)
    {
        delete pOriginator;
    }

    return 0;
}