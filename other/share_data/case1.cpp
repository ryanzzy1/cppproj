// 直接参数传递
#include <iostream>


class DataHolder 
{
public:
    int sharedValue = 0;
};

class ClassA 
{
public:
    void useDate(DataHolder& dh) {
        dh.sharedValue = 43;
    }
};

class ClassB 
{
public:
    void useDate(DataHolder& dh) {
        std::cout << "Shared value in ClassB: " << dh.sharedValue << std::endl;
    }
};

int main(){
    DataHolder dh;
    ClassA a;
    ClassB b;

    a.useDate(dh);
    b.useDate(dh);

    return 0;
}