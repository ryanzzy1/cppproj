// stack.h

#ifndef STACK_H_
#define STACK_H_

#include <iostream>

typedef unsigned long Item;

class Stack
{
private:
    enum {MAX = 10};
    Item items[MAX];
    int top;
public:
    Stack() : top(0) {}
    ~Stack() {}
    bool isempty() const {return top == 0;}
    bool isfull() const {return top == MAX;}
    bool push(const Item & item);
    bool pop(Item & item);
};

#endif