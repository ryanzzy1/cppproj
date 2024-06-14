
#include <iostream>

using namespace std;

#include <mutex>

class singleton{
private:
    singleton(){}

    static singleton* p;
    static mutex lock_;
public:
    singleton *instance();

    //trash recycle
    class CGarbo{
    public:
        ~CGarbo(){
            if(singleton::p)
                delete singleton::p;
        }
    };
    static CGarbo Garbo;  //
};

singleton *singleton::p = nullptr;

singleton::CGarbo Garbo;

singleton* singleton::instance(){
    if (p == nullptr) {
        lock_guard<mutex> guard(lock_);
        if (p == nullptr)
            p = new singleton();
    }
    return p;
}