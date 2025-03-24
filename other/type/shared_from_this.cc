#include <iostream>
#include <memory>
using namespace std;

class C : public enable_shared_from_this<C> {
public:
    C(int b = 10):a(b) {cout << "C constructor" << endl;}
    ~C() {cout << "C destructor" << endl;}
    void show()const {cout << "a = " << a << endl;}
    shared_ptr<C> object_ptr(){
        return shared_from_this();
    }
private:
    int a;
};

int main(){
    shared_ptr<C> p1(new C(20));
    p1->show();
    shared_ptr<C> p2 = p1->object_ptr();
    p2->show();
    cout << "p1.use_count() = " << p1.use_count() << endl;
    cout << "p2.use_count() = " << p2.use_count() << endl;
    return 0;
}