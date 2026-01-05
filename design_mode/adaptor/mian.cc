#include <iostream>
#include <iomanip>
#include <algorithm>

class Target
{
public:
    virtual ~Target() = default;
    virtual std::string Request() const {
        return "Target: 默认的请求处理方式。";
    }
};

class Adaptee
{
public:
    std::string SpecificRequest() const {
        return ".eetpadA eht fo roivaheb laicepS";
   }
};

class Adapter : public Target, public Adaptee
{
public:
    Adapter() {}
    std::string Request() const override {
        std::string to_reverse = SpecificRequest();
        std::reverse(to_reverse.begin(), to_reverse.end());

        return "Adaptor: (翻译) " + to_reverse;
    }
};

void ClientCode(const Target *target) {
    std::cout << target->Request() << std::endl;
}

int main()
{
    std::cout << "Client: I can work just fine with the Target objects:\n";
    Target *target = new Target;
    ClientCode(target);
    std::cout << "\n\n";
    Adaptee *adaptee = new Adaptee;
    std::cout << "Client: The Adaptee class has a weird interface. See, I don't understand it:\n";
    std::cout << "Adaptee: " << adaptee->SpecificRequest();
    std::cout << "\n\n";

    Adapter *adapter = new Adapter;
    std::cout << "Client: But I can work with it via the Adapter:\n";
    ClientCode(adapter);

    delete target;
    delete adaptee;
    delete adapter;

    return 0;
}

