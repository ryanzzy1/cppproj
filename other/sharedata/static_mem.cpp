#include <iostream>

class SharedResource {
public:
    static int counter;
    static void increment() {counter++; }
};

int SharedResource::counter = 0;

class ProcessorA {
public:
    void process() {
        SharedResource::increment();
        std::cout << "ProcessorA processed. Counter: " << SharedResource::counter << std::endl;
    }
};

class ProcessorB {
public:
    void process() {
        SharedResource::increment();
        std::cout << "ProcessorB processed. Counter: " << SharedResource::counter << std::endl;
    }
};

int main()
{
    ProcessorA a;
    ProcessorB b;

    a.process();
    b.process();

    return 0;
}