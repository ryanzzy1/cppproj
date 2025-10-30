#include <iostream>
#include <memory>

class SharedData {
public:
    int value = 40;
};

class ClassA {
private:
    std::shared_ptr<SharedData> shared; // Shared pointer member
public:
    ClassA(std::shared_ptr<SharedData> sd) : shared(sd) {} //
    void change() { shared->value = 100; }
};

class ClassB {
private:
    std::shared_ptr<SharedData> shared; // Shared pointer member
public:
    ClassB(std::shared_ptr<SharedData> sd) : shared(sd) {} //
    void show() {std::cout << "value: " << shared->value << std::endl; }
};

int main() {
    auto sharedObj = std::make_shared<SharedData>();

    ClassA a(sharedObj);
    ClassB b(sharedObj);

    a.change();
    b.show(); // Should print: value: 100

    return 0;
}