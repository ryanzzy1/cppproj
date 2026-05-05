#ifndef TSTACK_H_
#define TSTACK_H_

template <class Type>
class Stack
{
private:
    enum {MAX = 10};
    Type items[MAX];
    int top;
public:
    Stack();
    ~Stack() {}
    bool isempty() const {return top == 0;}
    bool isfull() const {return top == MAX;}
    bool push(const Type & item);
    bool pop(Type & item);
};

template <class Type>
Stack<Type>::Stack() 
{
    top = 0;
}

template <class Type>
bool Stack<Type>::push(const Type & item)
{
    if (top < MAX)
    {
        items[top++] = item;
        return true;
    }
    else 
        return false;
}

template <class Type>
bool Stack<Type>::pop(Type & item)
{
    if (top > 0)
    {
        item = items[--top];
        return true;
    }
    else
        return false;
}

#endif