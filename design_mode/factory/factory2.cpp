#include <iostream>
#include <vector>

using namespace std;

typedef enum productTypeTag
{
    TypeA,
    TypeB,
    TypeC
}PRODUCTTYPE;

class product
{
public:
    virtual void show() = 0;
};

class productA : public product
{
public:
    void show(){
        cout << "This is ProductA." << endl;
    }
};

class productB : public product
{
public:
    void show(){
        cout << "This is ProductB." << endl;
    }
};

class productC : public product
{
public:
    void show(){
        cout << "This is ProductC." << endl;
    }
};

class Factory
{
public:
    product* CreateProduct(PRODUCTTYPE type)
    {
        switch (type)
        {
        case TypeA:
            return new productA();
        case TypeB:
            return new productB();
        case TypeC: 
            return new productC();
        default:
            return NULL;            
        }
    }
};

int main()
{
    Factory* CreateFactory = new Factory();

    // productA* CreateProductA = CreateFactory->CreateProduct(TypeA);
    product* objA = CreateFactory->CreateProduct(TypeA);
    if(objA != NULL){
        objA->show();
    }

    product* objB = CreateFactory->CreateProduct(TypeB);
    if(objB){
        objB->show();
    }

    product* objC = CreateFactory->CreateProduct(TypeC);
    if(objC){
        objC->show();
    }

    delete CreateFactory;
    CreateFactory = NULL;

    delete objA;
    objA = NULL;

    delete objB;
    objB = NULL;

    delete objC;
    objC = NULL;

    return 0;
}
