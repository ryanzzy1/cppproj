#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Component
{
public:
    Component(string name) : m_strComname(name) {
        cout << "Component ctor: " << m_strComname << endl;
    }

    virtual ~Component() {
        cout << "Component dtor: " << m_strComname << endl;
    }

    virtual void Operation() = 0;
    virtual void Add(Component *) = 0;
    virtual void Remove(Component *) = 0;
    virtual Component *GetChild(int) = 0;
    virtual string GetName()
    {
        return m_strComname;
    }
    virtual void Print() = 0;

protected:
    string m_strComname;
};

class Leaf : public Component
{
public:
    Leaf(string name) : Component(name)
    {
        cout << "Leaf ctor." << endl;
    }

    void Operation()
    {
        cout << "It's " << m_strComname << endl;
    }

    void Add(Component *pComponent) {}
    void Remove(Component *pComponent) {}
    Component *GetChild(int index)
    {
        return nullptr;
    }

    void Print(){
        cout << "Leaf Print." << endl;
    }
};

class Composite : public Component
{
public:
    Composite(string name) : Component(name)
    {
        cout << "Composite ctor." << endl;
    }

    ~Composite()
    {
        vector<Component *>::iterator it = m_vecComp.begin();
        while(it != m_vecComp.end())
        {
            if (*it != nullptr)
            {
                cout << "----delete " << (*it)->GetName() << endl;
                delete *it;
                *it = nullptr;
            }
            m_vecComp.erase(it);
            it = m_vecComp.begin();
        }
    }

    void Operation()
    {
        cout << "This is " << m_strComname << endl;
    }

    void Add(Component *pComponent)
    {
        m_vecComp.push_back(pComponent);
    }

    void Remove(Component *pComponent)
    {
        for (vector<Component *>::iterator it = m_vecComp.begin(); it != m_vecComp.end(); ++it)
        {
            if ((*it)->GetName() == pComponent->GetName())
            {
                if (*it != nullptr)
                {
                    cout << "----delete " << (*it)->GetName() << endl;
                    delete *it;
                    *it = nullptr;
                }
                m_vecComp.erase(it);
                break;
            }
        }
    }

    Component *GetChild(int index)
    {
        if (index > m_vecComp.size())
        {
            return nullptr;
        }
        return m_vecComp[index - 1];
    }

    void Print()
    {
        for (vector<Component *>::iterator it = m_vecComp.begin(); it != m_vecComp.end(); ++it)
        {
            cout << (*it)->GetName() << endl;
        }
    }

private:
    vector<Component *> m_vecComp;
};

int main()
{
    Component *pNode = new Composite("BJ head office");
    Component *pNodeHr = new Composite("HR");
    Component *pNodecd = new Composite("SH");
    Component *pNodesh = new Composite("CD");
    Component *pNodeBt = new Composite("BT");
    pNode->Add(pNodeHr);
    pNode->Add(pNodesh);
    pNode->Add(pNodecd);
    pNode->Add(pNodeBt);
    pNode->Print();

    Component *pSubNodeshHr = new Leaf("SH HR");
    Component *pSubNodeshCg = new Leaf("SH Purching Department");
    Component *pSubNodeshXs = new Leaf("SH Sales Department");
    Component *pSubNodeshZb = new Leaf("SH QA Department");
    pNodesh->Add(pSubNodeshHr);
    pNodesh->Add(pSubNodeshCg);
    pNodesh->Add(pSubNodeshXs);
    pNodesh->Add(pSubNodeshZb);
    pNode->Print();

    pNodesh->Remove(pSubNodeshZb);
    if (pNode != nullptr)
    {
        delete pNode;
        pNode = nullptr;
    }

    return 0;
}
