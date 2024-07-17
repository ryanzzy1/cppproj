// details refer to : https://developer.aliyun.com/article/1258697?spm=a2c6h.24874632.expert-profile.42.6bdb2b601k4W0v

#include <iostream>

using namespace std;

// A Product class 

class Product
{
public:
    virtual void show() = 0;
};

// A ConcreteProduct class
class ConcreteProduct : public Product
{
public:
    void show() {
        cout << "This is a concretProduct object" << endl;
    }
};

// a Cretaor abstract class
class Creator
{
public:
    virtual Product * createProduct() = 0;
};

// a ConcreteCreator class
class ConcreteCreator : public Creator
{
public:
    Product* createProduct()
    {
        return new ConcreteProduct();
    }
};

// Client

int main()
{
    Creator* creator = new ConcreteCreator();
    Product* product = creator->createProduct();
    product->show();

    return 0;
}