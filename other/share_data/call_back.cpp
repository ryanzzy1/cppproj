#include <thread>
#include <mutex>
#include <iostream>
#include <functional>

using namespace std;

class B {

private:

    std::mutex mtx_;

public:

    void param_handle(int param) {
        std::lock_guard<std::mutex> lock(mtx_);
        cout << "B::param_handle: " << param << endl;
    }
    
    bool gendata(){
        return true;
    }

};

class A {

private:
    std::function<void(int)> callback_;

public:

    A (std::function<void(int)> callback) : callback_(callback) {}
    
    void genparam(){
        while(true) {
            int param = rand() % 100;

            if (param) {
                cout << "A::param: " << param << endl;
            }

            if (callback_){
                callback_(param);
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

};


int main()
{

    B b;

    A a(std::bind(&B::param_handle, &b, std::placeholders::_1));

    std::thread threada(&A::genparam, &a);
    // std::thread threadb(&B::gendata, &b);

    threada.join();
    // threadb.join();

    return 0;
}
