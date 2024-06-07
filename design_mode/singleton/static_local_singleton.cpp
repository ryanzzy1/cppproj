#include <iostream>

class singleton{
private:
    static singleton* p;
    singleton(){}
public:
    singleton* instance();
};

singleton* singleton::instance(){
    static singleton p; //?
    return &p;
}