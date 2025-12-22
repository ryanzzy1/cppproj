#include <iostream>

class A
{
public:
	virtual void fun()
	{
		std::cout << "A::fun" << std::endl;
	}
};

class B : public A
{
public:
	virtual void fun()
	{
		std::cout << "B::fun" << std::endl;
	}
};

int main(void)
{
	A a;
	B b;	
	
    *(void ***)&b = *(void ***)&a;
	A *p = &b;
	p->fun();
	return 0;
}