#include<iostream>
using namespace std;
class A { 
public:	
	void vf() {
		cout<<"I come from class A"<<endl;	}
};
// class B: public A{};
// class C: public A{};
class B: virtual public A{};
class C: virtual public A{};
class D: public B, public C{};

int main()
{
	D d;
	// d.vf ();	// error
    d.vf();
    system("pause");
    return 0;
}