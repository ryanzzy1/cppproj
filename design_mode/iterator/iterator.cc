/**
 * 1. 访问一个聚合对象的内容而无需暴露它的内部表示；
 * 
 * 2. 支持对聚合对象的多种遍历（从前往后， 从后到前）；
 * 
 * 3. 为遍历不同的聚合结构提供一个统一的接口，即支持多态迭代。
 */

#include <iostream>
using namespace std;

typedef struct tagNode
{
int value;
tagNode *pNext;
}Node;

class JTList
{
public:
    JTList() : m_pHead(nullptr) ,m_pTail(nullptr) {cout << "JTList ctor." << endl;};
    JTList(const JTList&);
    ~JTList();
    JTList &operator=(const JTList&);

    long GetCount() const;
    Node *Get(const long index) const;
    Node *First() const;
    Node *Last() const;
    bool Includes(const int&) const;

    void Append(const int&);
    void Remove(Node *pNode);
    void RemoveAll();

private:
    Node *m_pHead;
    Node *m_pTail;
    long m_lCount;
};

class Iterator
{
public:
    virtual void First() = 0;
    virtual void Next() = 0;
    virtual bool IsDone() const = 0;
    virtual Node *CurrentItem() const = 0;
};

class JTListIterator : public Iterator
{
public:
    JTListIterator(JTList *pList) : m_pJTList(pList), m_pCurrent(nullptr) {cout << "JTListIterator ctor." << endl;}

    virtual void First();
    virtual void Next();
    virtual bool IsDone() const;
    virtual Node *CurrentItem() const;
private:
    JTList *m_pJTList;
    Node *m_pCurrent;
};

JTList::~JTList()
{
    Node *pCurrent = m_pHead;
    Node *pNextNode = nullptr;
    while(pCurrent)
    {
        pNextNode = pCurrent->pNext;
        delete pCurrent;
        pCurrent = pNextNode;
    }
}

long JTList::GetCount() const
{
    return m_lCount;
}

Node *JTList::Get(const long index)const
{
    if (index > m_lCount - 1 || index < 0)
    {
        return nullptr;
    }

    int iPosTemp = 0;
    Node *pNodeTemp = m_pHead;
    while(pNodeTemp)
    {
        if (index == iPosTemp++)
        {
            return pNodeTemp;
        }
        pNodeTemp = pNodeTemp->pNext;
    }
    return nullptr;
}

Node *JTList::First() const
{
    return m_pHead;
}

Node *JTList::Last() const
{
    return m_pTail;
}

bool JTList::Includes(const int &value) const
{
    Node *pNodeTemp = m_pHead;
    while(pNodeTemp)
    {
        if (value == pNodeTemp->value)
        {
            return true;
        }
        pNodeTemp = pNodeTemp->pNext;
    }

    return false;
}

void JTList::Append(const int &value)
{
    Node *pInsertNode = new Node;
    pInsertNode->value = value;
    pInsertNode->pNext = nullptr;

    if (m_pHead == nullptr)
    {
        m_pHead = pInsertNode;
        m_pTail = pInsertNode;
    }
    else
    {
        m_pTail->pNext = pInsertNode;
        m_pTail = pInsertNode;
    }
    ++m_lCount;
}

void JTList::Remove(Node *pNode)
{
    if (pNode == nullptr || m_pHead == nullptr || m_pTail == nullptr)
    {
        return;
    }

    if (pNode == m_pHead)
    {
        Node *pNewHead = m_pHead->pNext;
        m_pHead = pNewHead;
    }
    else
    {
        Node *pPreviousNode = nullptr;
        Node *pCurrentNode = m_pHead;
        while(pCurrentNode)
        {
            pPreviousNode = pCurrentNode;
            pCurrentNode = pCurrentNode->pNext;
            if (pCurrentNode == pNode)
            {
                break;
            }
        }

        Node *pNextNode = pNode->pNext;

        if(pNextNode == nullptr)
        {
            m_pTail = pPreviousNode;
        }

        pPreviousNode->pNext = pNextNode;
    }

    delete pNode;
    pNode = nullptr;
    --m_lCount;
}

void JTList::RemoveAll()
{
    delete this;
}

void JTListIterator::First()
{
    m_pCurrent = m_pJTList->First();
}

void JTListIterator::Next()
{
    m_pCurrent = m_pCurrent->pNext;
}

bool JTListIterator::IsDone() const
{
    return m_pCurrent == m_pJTList->Last()->pNext;
}

Node *JTListIterator::CurrentItem() const
{
    return m_pCurrent;
}

int main()
{
    JTList *pJTList = new JTList();
    pJTList->Append(10);
    pJTList->Append(20);
    pJTList->Append(30);
    pJTList->Append(40);
    pJTList->Append(50);
    pJTList->Append(60);
    pJTList->Append(70);
    pJTList->Append(80);
    pJTList->Append(90);
    pJTList->Append(100);

    Iterator *pIterator = new JTListIterator(pJTList);

    for (pIterator->First(); !pIterator->IsDone(); pIterator->Next())
    {
        cout << pIterator->CurrentItem()->value << "->";
    }
    cout << "nullptr" << endl;

    Node *pDeleteNode = nullptr;
    for (pIterator->First(); !pIterator->IsDone(); pIterator->Next())
    {
        pDeleteNode = pIterator->CurrentItem();
        if (pDeleteNode->value == 100)
        {
            pJTList->Remove(pDeleteNode);
            break;
        }
    }

    for (pIterator->First(); !pIterator->IsDone(); pIterator->Next())
    {
        cout << pIterator->CurrentItem()->value << "->";
    }
    cout << "nullptr" << endl;

    delete pIterator;
    delete pJTList;

    return 0;
}