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
        for(int i = 0; i < 5; ++i) {            
            std::cout << "Shared value in ClassA: " << dh.sharedValue << std::endl;
            dh.sharedValue++;
        }
        
    }
};

class ClassB 
{
public:
    void useDate(DataHolder& dh) {
        for(int i = 0; i < 5; ++i) {
            std::cout << "Shared value in ClassB: " << dh.sharedValue << std::endl;
            if (dh.sharedValue > 47) break; // 防止无限循环
        }
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