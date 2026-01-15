#include <iostream>

class AbstractClass {
public:
    void TemplateMethod() const {
        this->BaseOperation1();
        this->RequiredOperation1();
        this->BaseOperation2();
        this->Hook1();
        this->RequiredOperation2();
        this->BaseOperation3();
        this->Hook2();
    }

protected:
    void BaseOperation1() const {
        std::cout << "AbstractClass: BaseOperation1, i am doing the bulk of the work;\n";
    }

    void BaseOperation2() const {
        std::cout << "AbstractClass: BaseOperation2, But I let subclasses override some operations;\n";
    }

    void BaseOperation3() const {
        std::cout << "AbstractClass: BaseOperation3, But I am doing the bulk of the work anyway;\n";
    }

    virtual void RequiredOperation1() const = 0;

    virtual void RequiredOperation2() const = 0;

    virtual void Hook1() const {}

    virtual void Hook2() const {}
};

class ConcreteClass1 : public AbstractClass {
protected:
    void RequiredOperation1() const override {
        std::cout << "ConcreteClass1: Implemented Operation1;\n";
    }

    void RequiredOperation2() const override {
        std::cout << "ConcreteClass1: Implemented Operation2;\n";
    }
};

class ConcreteClass2 : public AbstractClass {
protected:
    void RequiredOperation1() const override {
        std::cout << "ConcreteClass2: Implemented Operation1;\n";
    }

    void RequiredOperation2() const override {
        std::cout << "ConcreteClass2: Implemented Operation2;\n";
    }

    void Hook1() const override {
        std::cout << "ConcreteClass2: Overridden Hook1;\n";
    }
};

void ClientCode(AbstractClass* obj) {
    // 使用 AbstractClass 的接口
    obj->TemplateMethod();
}
int main() 
{
    std::cout << "Same client code can work with different subclasses:\n";
    ConcreteClass1* obj1 = new ConcreteClass1;
    ClientCode(obj1);
    delete obj1;

    std::cout << "\n";

    std::cout << "Same client code can work with different subclasses:\n";
    ConcreteClass2* obj2 = new ConcreteClass2;
    ClientCode(obj2);
    delete obj2;

    return 0;
}