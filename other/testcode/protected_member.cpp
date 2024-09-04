#include<iostream>

using namespace std;

class B
{	
    private: 
        int i;
	protected: 
        int j;
	public: 
        int k;
        int get_i(int a){
            i = a;
            cout << "i:" << i << endl;
            return 0;
        }
        int get_j(int a) {
            j = a;
            cout << "j:" << j << endl;
            return 0;
        }
};

class D: public B
{
public:
	void f() 
	{	
        // i=1;//cannot access �����಻�ɷ��ʻ���˽�г�Ա
		j=2;//��������Է��ʻ��ౣ����Ա	
        k=3;	
    }
};

int main()
{	
    B b; 
	// b.i=1;//cannot access ˽�г�Ա�����ʹ���߲��ܷ���
    b.get_i(1);
	// b.j=2; //cannot access ������Ա�����ʹ���߲��ܷ���
    b.get_j(2);
	b.k=3;
    system("pause");
    return 0;
}
