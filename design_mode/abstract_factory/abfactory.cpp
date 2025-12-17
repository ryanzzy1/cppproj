#include <iostream>

using namespace std;

class Product
{
public:
    virtual void Show() = 0;
};

class ProductA : public Product
{
public:
    virtual void Show()
    {
        cout << "ProductA Show called." << endl;
    }
};

class ProductB : public Product
{
public:
    virtual void Show()
    {
        cout << "ProductB Show called." << endl;
    }
};

class Factory
{
public:
    virtual Product *CreateProductA() = 0;

    virtual Product *CreateProductB() = 0;
};

class ConcreteFactoryA : public Factory
{
public:
    virtual Product *CreateProductA()
    {
        return new ProductA;
    }
    
    virtual Product *CreateProductB()
    {
        return new ProductB;
    }
};

class ConcreteFactoryB : public Factory
{
public:
    virtual Product *CreateProductA()
    {
        return new ProductA;
    }

    virtual Product *CreateProductB()
    {
        return new ProductB;
    }
};

int main()
{
    Factory *pFactory = nullptr;
    Product *pProductA = nullptr;
    Product *pProductB = nullptr;  

    cout << "========= ProductA =======" << endl;
    pFactory = new ConcreteFactoryA;
    pProductA = pFactory->CreateProductA();
    pProductB = pFactory->CreateProductB();
    pProductA->Show();
    pProductB->Show();
    
    delete pProductB;
    delete pProductA;    
    delete pFactory;

    cout << "========== ProductB ========" << endl;
    pFactory = new ConcreteFactoryB;
    pProductA = pFactory->CreateProductA();
    pProductB = pFactory->CreateProductB();
    pProductA->Show();
    pProductB->Show();
    delete pProductB;
    delete pProductA;
    delete pFactory;    

    return 0;
}