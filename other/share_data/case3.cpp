// 静态成员变量 （全局共享）

#include <iostream>

class GlobalData {
public:
    static inline int counter = 0; // C++14 inline static member initialization
};

class Incrementer {
public:
    void increment() {
        GlobalData::counter++;
        std::cout << "Incrementer: counter = " << GlobalData::counter << std::endl;
    }
}; 

class Printer {
public:
    void print() {
        std::cout << "Printer: counter = " << GlobalData::counter << std::endl;
    }
};

int main() {
    Incrementer inc;
    Printer print;

    inc.increment(); // Increment the counter
    print.print();   // Print the current value of the counter

    inc.increment(); // Increment again
    print.print();   // Print the updated value of the counter

    return 0;
}