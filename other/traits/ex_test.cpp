#include <iostream>
using namespace std;
class Test1
{
public :
	Test1(int num):n(num){}
	int get_num(){return n;}
private:
	int n;
};
class Test2
{
public :
	explicit Test2(int num):n(num){}
	int get_num(){return n;}
private:
	int n;
};
 
int main()
{
	Test1 t1 = 12;
	std::cout << "t1: param: " << t1.get_num() << std::endl;
	Test2 t2(13);
	std::cout << "t2: param: " << t2.get_num() << std::endl;
//	Test2 t3 = 14;

	return 0;
}
