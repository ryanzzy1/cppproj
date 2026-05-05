// brass.h
#ifndef BRASS_H_
#define BRASS_H_
#include <iostream>

class Brass
{
private:
    std::string customerName;
    int accountNum;
    double balance;
public:
    Brass();
    Brass(const std::string & s, int an, double bal);
    void Deposit(double money);
    virtual void Withdraw(double money);
    double Balance() const;
    virtual void ViewAcct() const;
    virtual ~Brass() {}
};

#endif