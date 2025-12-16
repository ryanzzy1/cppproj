/**
 * 工厂方法模式适用于产品种类结构单一的场合，为一类产品提供创建的接口；
 * 
 * 而抽象工厂方法使用于产品种类结构多的场合，主要用于创建一组相关或相互依赖的产品对象；
 */

#include <iostream>

using namespace std;

class PhoneInterface
{
public:
    virtual void print_brand() = 0;
};

// 美版iphone
class UsApple : public PhoneInterface
{
public:
    virtual void print_brand()
    {
        cout << "美版的iphone." << endl;
    }
};

// 美版mi
class UsMi : public PhoneInterface
{
public:
    virtual void print_brand()
    {
        cout << "美版Mi." << endl;
    }
};

// 大陆版iphone
class ChinaApple : public PhoneInterface
{
public:
    virtual void print_brand()
    {
        cout << "大陆版的iphone." << endl;
    }
};

// 大陆版mi
class ChinaMi : public PhoneInterface
{
public:
    virtual void print_brand()
    {
        cout << "大陆版Mi." << endl;
    }
};

// 抽象工厂
class FactoryInterface
{
public:
    // product1
    virtual PhoneInterface *production_apple() = 0;
    // product2
    virtual PhoneInterface *production_mi() = 0;
};

// 大陆工厂
class ChinaFactory : public FactoryInterface
{
public:
    // product_apple
    virtual PhoneInterface *production_apple()
    {
        return new ChinaApple;
    }

    // product_mi
    virtual PhoneInterface *production_mi()
    {
        return new ChinaMi;
    }

};

// 美版工厂
class UsFactory : public FactoryInterface
{
public:
    // product_apple
    virtual PhoneInterface *production_apple()
    {
        return new UsApple;
    }

    // product_mi
    virtual PhoneInterface *production_mi()
    {
        return new UsMi;
    }

};

int main()
{
    FactoryInterface *pFactory = nullptr;
    PhoneInterface *pPhone = nullptr;

    // 大陆版的手机工厂
    cout << "=====MainlandPhoneFactory========" << endl;
    pFactory = new ChinaFactory;

    // 大陆版的iphone
    pPhone = pFactory->production_apple();
    pPhone->print_brand();
    delete pPhone;
    // 大陆版的mi
    pPhone = pFactory->production_mi();
    pPhone->print_brand();
    delete pPhone;
    delete pFactory;

    // 美版手机工厂
    cout << "=====UsPhoneFactory========" << endl;
    pFactory = new UsFactory;
    // 美版的iphone
    pPhone = pFactory->production_apple();
    pPhone->print_brand();
    delete pPhone;
    // 美版的mi
    pPhone = pFactory->production_mi();
    pPhone->print_brand();
    delete pPhone;
    delete pFactory;

    return 0;
}