// stack.h
#ifndef STACK_H_
#define STACK_H_

typedef unsigned long Item;

class Stack
{
private:
    enum {MAX = 10};    //符号常量
    Item items[MAX];    //栈存储区
    int top;            // 栈顶指针
public:
    Stack();
    bool isempty() const;
    bool isfull() const;
    bool push(const Item & item);   // 将item添加到栈中
    bool pop(Item & item);        // 从栈中弹出item
};

#endif // STACK_H_