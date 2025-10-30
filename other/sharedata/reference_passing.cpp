#include <iostream>

class SharedData {
public:
    int value = 0;
    void print() {std::cout << "shared value:" << value << std::endl;}
};

class ClassA {
private:
    SharedData& shared; // Reference member
public:
    ClassA(SharedData& sd) : shared(sd) {} // Constructor initializes reference
    void modify(int v) { shared.value = v; }
    void show() { shared.print(); }
};

class ClassB {
private:
    SharedData& shared; // Reference member
public:
    ClassB(SharedData& sd) : shared(sd) {} // Constructor initializes reference
    void display() { shared.print(); }
};

int main() {
    SharedData commonData;

    ClassA objA(commonData);
    ClassB objB(commonData);

    objA.modify(42);
    objA.show(); // Should print: shared value:42
    objB.display(); // Should also print: shared value:42
}