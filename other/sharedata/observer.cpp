#include <iostream>
#include <vector>
#include <algorithm>

class DataSubject;

class Observer {
public:
    virtual void update(int value) = 0;
    virtual ~Observer() = default;
};

class DataSubject {
private:
    int value = 0;
    std::vector<Observer*> observers;

    void notifyObservers() {
        for (auto obs : observers) {
            obs->update(value);
        }
    }
public:
    void addObserver(Observer* obs) {observers.push_back(obs);}
    void removeObserver(Observer* obs) {
        observers.erase(std::remove(observers.begin(), observers.end(), obs), observers.end());
    }

    void setValue(int v) {
        value = v;
        notifyObservers();
    }
};

class DispalyA : public Observer {
public:
    void update(int v ) override {
        std::cout << "DispalyA received: " << v << std::endl;
    }
};

class DispalyB : public Observer {
    void update(int v ) override {
        std::cout << "DispalyB received: " << v << std::endl;
    }
};

int main() {
    DataSubject subject;
    DispalyA displayA;
    DispalyB displayB;

    subject.addObserver(&displayA);
    subject.addObserver(&displayB);

    // subject.setValue(10);
    // subject.setValue(20);

    // subject.removeObserver(&displayA);
    subject.setValue(42);

    return 0;
}