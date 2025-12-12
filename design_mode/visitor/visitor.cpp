/**
 * 访问者模式使用场合：
 * 
 * 1. 一个对象结构包含很多类对象，它们有不同的接口，而你想对这些对象实施一些依赖于其具体类的操作；
 * 
 * 2. 需要对一个对象结构中的对象进行很多不同的并且不相关的操作，而你想避免让这些操作“污染”这些对象的类，
 *    visitor模式可以将这些操作封装集中起来定义在一个类中；
 * 
 * 3. 当该对象结构被很多应用共享时，使用visitor模式可以避免在每个应用中都实现这些操作；
 * 
 * 4. 定义对象结构的类很少改变，但经常需要在此结构上定义新操作。改变对象结构类需要重新定义对所有访问者的
 *    接口。
 * 
 */

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class CCElementA;
class CCElementB;

class visitor 
{
public:
    virtual void visitCCElementA(CCElementA *elementA) = 0;
    virtual void visitCCElementB(CCElementB *elementB) = 0;
};

class CCvisitor1 : public visitor
{
public:
    void visitCCElementA(CCElementA *elementA);
    void visitCCElementB(CCElementB *ElementB);
};

void CCvisitor1::visitCCElementA(CCElementA *elementA)
{
    cout << "CCvisitor1 visit CCElementA" << endl;

}

void CCvisitor1::visitCCElementB(CCElementB *ElementB)
{
    cout << "CCvisitor1 visit CCElementB" << endl;

}

class CCvisitor2 : public visitor
{
public:
    void visitCCElementA(CCElementA *elementA);
    void visitCCElementB(CCElementB *ElementB);
};

void CCvisitor2::visitCCElementA(CCElementA *elementA)
{
    cout << "CCvisitor2 visit CCElementA" << endl;
}

void CCvisitor2::visitCCElementB(CCElementB *ElementB)
{
    cout << "CCvisitor2 visit CCElementB" << endl;
}

class Element
{
public:
    virtual void Accept(visitor *pVisitor) = 0;
};

class CCElementA : public Element
{
public:
    void Accept(visitor *pVisitor);
};

void CCElementA::Accept(visitor *pVisitor)
{
    pVisitor->visitCCElementA(this);
}

class CCElementB : public Element
{
public:
    void Accept(visitor *pVisitor);
};

void CCElementB::Accept(visitor *pVisitor)
{
    pVisitor->visitCCElementB(this);
}

class ObjectStructure
{
public:
    void attach(Element *pelement);
    void detach(Element *pelement);
    void Accept(visitor *pVisitor);
private:
    vector<Element *> elements;
};

void ObjectStructure::attach(Element *pelement)
{
    elements.push_back(pelement);
}

void ObjectStructure::detach(Element *pelement)
{
    // vector<Element *>::iterator it = find(elements.begin(), elements.end(), pelement);
    // if (it != elements.end()) {
    //     elements.erase(it);
    // }

    for (vector<Element *>::iterator it = elements.begin(); it != elements.end(); ++it)
    {
        elements.erase(it);
    }
}
void ObjectStructure::Accept(visitor *pVisitor)
{
    // for (auto &element : elements) {
    //     element->Accept(pVisitor);
    // }
    for (vector<Element *>::iterator it = elements.begin(); it != elements.end(); ++it)
    {
        (*it)->Accept(pVisitor);
    }
}

int main()
{
    ObjectStructure *pObj = new ObjectStructure; // 对象结构体

    CCElementA *pElementA = new CCElementA; // 元素A
    CCElementB *pElementB = new CCElementB; // 元素B

    pObj->attach(pElementA);
    pObj->attach(pElementB);

    CCvisitor1 *pVisitor1 = new CCvisitor1; // 访问者1
    CCvisitor2 *pVisitor2 = new CCvisitor2; // 访问者2

    pObj->Accept(pVisitor1);
    pObj->Accept(pVisitor2);

    if(pVisitor2) delete pVisitor2;
    if(pVisitor1) delete pVisitor1;
    if(pElementB) delete pElementB;
    if(pElementA) delete pElementA;
    if(pObj) delete pObj;

    return 0;
}