// 观察者模式

#include <iostream>
#include <vector>
#include <functional>

class DataSubject {
    int value;
    std::vector<std::function<void(int)>> observers;
public:
    void attach(std::function<void(int)> observer) {
        observers.push_back(observer);
    }

    void setValue(int v) {
        value = v;
        notify();
    }

    void notify() {
        for (auto& obs : observers) {
            // 调用每个观察者的回调函数, 是std::function<void(int)>::operator()方法
            obs(value); // 通知所有观察者
        }
    }
};

class ObserverA {
public:
    void react(int val) {
        std::cout << "ObserverA reacting to value: " << val << std::endl;
    }
};

int main()
{
    
    DataSubject subject;
    ObserverA observerA;

    subject.attach([&observerA](int val) { observerA.react(val); });
    
    subject.setValue(10);

    return 0;    

}