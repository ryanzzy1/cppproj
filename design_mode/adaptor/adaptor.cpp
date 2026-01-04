/**
 * 使用某个类，但接口与其他代码不兼容时，可以使用适配器类
 * 
 * 复用处在同一个继承体系，他们有一些额外的共同方法，但共同方法不是所有在这一继承体系中的子类所具有的共性；
 * 
 * 
 */

#include <iostream>
#include <algorithm>

using namespace std;

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

class Adapter : public Target
{
private:
    Adaptee* adaptee_;
public:
    Adapter(Adaptee* adaptee) : adaptee_(adaptee) {}
    std::string Request() const override {
        std::string to_reverse = this->adaptee_->SpecificRequest();
        std::reverse(to_reverse.begin(), to_reverse.end());

        return "Adaptor: (翻译) " + to_reverse;
    }
};


void ClientCode(const Target *target) {
    cout << target->Request() << std::endl;
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
  std::cout << "Client: But I can work with it via the Adapter:\n";
  Adapter *adapter = new Adapter(adaptee);
  ClientCode(adapter);
  std::cout << "\n";

  delete target;
  delete adaptee;
  delete adapter;
    return 0;
}