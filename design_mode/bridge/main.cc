#include <iostream>
#include <algorithm>
#include <string>


class Implementation {
public:
    virtual ~Implementation() {}
    virtual std::string OperationImplementation() const = 0;
};

class ConcreteImplementationA : public Implementation {
public:
    std::string OperationImplementation() const override {
        return "ConcreteImplementationA: 具体实现A的操作。\n";
    }
};

class ConcreteImplementationB : public Implementation {
public:
    std::string OperationImplementation() const override {
        return "ConcreteImplementationB: 具体实现B的操作。\n";
    }
};

class Abstraction {
protected:
    Implementation* implementation_;

public:
    Abstraction(Implementation* implementation) : implementation_(implementation) {

    }

    virtual ~Abstraction() {}

    virtual std::string Operation() const {
        return "Abstraction: 基类操作与:\n" + this->implementation_->OperationImplementation();
    }
};

class ExtendedAbstraction : public Abstraction {
public:
    ExtendedAbstraction(Implementation* implementation) : Abstraction(implementation) {

    }
    std::string Operation() const override {
        return "ExtendedAbstraction: 扩展操作与:\n" + this->implementation_->OperationImplementation();
    }
};

void ClientCode(const Abstraction& abstraction) {
    std::cout << abstraction.Operation();
}

int main() 
{
    Implementation* implementation = new ConcreteImplementationA;
    Abstraction* abstraction = new Abstraction(implementation);
    ClientCode(*abstraction);

    delete abstraction;
    delete implementation;

    implementation = new ConcreteImplementationB;
    abstraction = new ExtendedAbstraction(implementation);
    ClientCode(*abstraction);

    delete abstraction;
    delete implementation;

    return 0;
}