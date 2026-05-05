// useless.cpp
#include <iostream>
using namespace std;

class Useless
{
private:
    int n;
    char * pc;
    static int ct;
    void ShowObject() const;
public:
    Useless();
    explicit Useless(int k);
    Useless(int k, char ch);
    Useless(const Useless & f);
    Useless(Useless && f);
    ~Useless();
    Useless operator+(const Useless & f) const;
    // operator=() is needed in copy and move versions
    void ShowData() const;
};

int Useless::ct = 0;

Useless::Useless()
{
    ++ct;
    n = 0;
    pc = nullptr;
    cout << "此处调用默认构造函数； 对象数量为：" << ct << endl;
    ShowObject();
}

Useless::Useless(int k) : n(k)
{
    ++ct;
    cout << "整型参数的构造函数被调用；对象数量为：" << ct << endl;
    pc = new char[n];
    ShowObject();
}

Useless::Useless(int k, char ch) : n(k)
{
    ++ct;
    cout << "一个整型参数和一个字符型参数的构造函数被调用；对象数量为：" << ct << endl;
    pc = new char[n];
    for (int i = 0; i < n; i++)
        pc[i] = ch;
    ShowObject();
}

Useless::Useless(const Useless & f) : n(f.n)
{
    ++ct;
    cout << "拷贝构造函数被调用，对象数量为：" << ct << endl;
    pc = new char[n];
    for (int i = 0; i < n; i++)
        pc[i] = f.pc[i];
    ShowObject();
}

Useless::Useless(Useless && f) : n(f.n)
{
    ++ct;
    cout << "移动构造函数被调用，对象数量为：" << ct << endl;
    pc = f.pc;
    f.pc = nullptr;
    f.n = 0;
    ShowObject();    
}

Useless::~Useless()
{
    cout << "析构函数被调用；对象剩余数量为：" << --ct << endl;
    cout << "删除对象：" << endl;
    ShowObject();
    delete [] pc;
}

Useless Useless::operator+(const Useless & f) const
{
    cout << "进入重载\"+\"法运算符, operator+()\n";
    Useless temp = Useless(n + f.n);
    for (int i = 0; i < n; i++)
        temp.pc[i] = pc[i];
    for (int i = n; i < temp.n; i++)
        temp.pc[i] = f.pc[i - n];
    cout << "temp object:\n";
    cout << "离开operator+()\n";
    return temp; // 需要手动开启-fno-elide-constructors, 否则编译器会自动优化返回值，将临时对象直接构造生成外部对象，省略移动构造函数。
}

void Useless::ShowObject() const
{
    cout << "元素数量为：" << n;
    cout << " 数据地址为：" << (void *) pc << endl;
}

void Useless::ShowData() const
{
    if (n == 0)
        cout << "对象是空的。" << endl;
    else 
        for (int i = 0; i < n; i++)
            cout << pc[i];
    cout << endl;
}

int main()
{
    {
        Useless one(10, 'x');
        Useless two = one;
        Useless three(20, 'o');
        Useless four(one + three);  // 期望调用拷贝构造函数
        // Useless four (one + three);
        cout << "对象One: ";
        one.ShowData();
        cout << "对象two: ";
        two.ShowData();
        cout << "对象three: ";
        three.ShowData();
        cout << "对象four: ";
        four.ShowData();
    }
}