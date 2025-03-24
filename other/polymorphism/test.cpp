#include <iostream>

class Base{
public:
    Base() = default;
    virtual void pureVirtual() = 0;
    void show(){
        std::cout << "Base::show()" << std::endl;
    }
};

class Derived : public Base{
public:
    Derived() :Base(){
        std::cout << "Derived::Derived()" << std::endl;
    }
};

class Derived2 : public Derived{
public:
    void pureVirtual() override{
        std::cout << "Derived2::pureVirtual()" << std::endl;
    }
};

int main()
{
    Derived2 d2;
    d2.pureVirtual();
    d2.show();
    return 0;
}