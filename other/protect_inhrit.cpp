#include <iostream>

using namespace std;

class Base{
    int x;
protected:
    int getx(){ return x; }
public:
    void setx(int n){ x=n; }
    void showx(){ cout<<x<<endl; }
};
class Derived:protected Base{
    int y;
public:
    void sety(int n){ y=n; }
    void sety(){ y=getx();}   //
    void showy(){ cout<<y<<endl; }
};
int main(){
    Derived obj;
    // obj.setx(10);    //protect interit cannot access
    obj.sety(20);
    // obj.showx();	 //same to previous
    obj.showy(); 
    system("pause");   
}