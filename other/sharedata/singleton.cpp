#include <iostream>

class Singleton {
public:
    static Singleton& getInstace() {
        static Singleton instance;
        return instance;
    }

    int data = 0;
    void print() {std::cout << "Singleton data: " << data << std::endl;}

    Singleton(const Singleton&) = delete;
    void operator=(const Singleton&) = delete;


private:
    Singleton(){}
};

class UserA {
public:
    void modify() {Singleton::getInstace().data = 55;}
};

class UserB {
public:
    void display() {Singleton::getInstace().print();}
};

int main() 
{
    UserA a;
    UserB b;

    a.modify();
    b.display();

    return 0;
}