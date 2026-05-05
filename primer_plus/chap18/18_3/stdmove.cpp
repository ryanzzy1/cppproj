#include <iostream>
#include <utility>

using std::cout;
using std::endl;

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
    Useless & operator=(const Useless & f); // copy assignment
    Useless & operator=(Useless && f);      // move assignment
    void ShowData() const;
};

int Useless::ct = 0;

Useless::Useless()
{
    cout << "无参构造函数被调用！" << endl;
    ++ct;
    n = 0;
    pc = nullptr;
}

Useless::Useless(int k) : n(k)
{
    cout << "一个整型参数的构造函数被调用！" << endl;
    
    ++ct;
    pc = new char[n];
}

Useless::Useless(int k, char ch) : n(k)
{
    cout << "一个整型和一字符型参数的构造函数被调用！" << endl;
    ++ct;
    pc = new char[n];
    for (int i = 0; i < n; i++)
        pc[i] = ch;
}

Useless::Useless(const Useless & f) : n(f.n)
{
    cout << "拷贝构造函数被调用！" << endl;
    ++ct;
    pc = new char[n];
    for (int i = 0; i < n; i++)
        pc[i] = f.pc[i];
}

Useless::Useless(Useless && f) : n(f.n)
{
    cout << "移动构造函数被调用！" << endl;
    ++ct;
    pc = f.pc;
    f.n = 0;
    f.pc = nullptr;
}

Useless::~Useless()
{
    cout << "析构函数被调用！" << endl;
    
    // --ct;
    delete [] pc;
}

Useless Useless::operator+(const Useless & f) const
{
    cout << "operator+()函数被调用！" << endl;

    // method 1
    // Useless temp = Useless(n + f.n);    // 调用Useless(int k) 构造函数，会对pc 分配内存
        // method 2
    Useless temp;   // 默认构造函数未分配内存，必须手动分配
    temp.n = n + f.n;
    temp.pc = new char [temp.n];
    
    for (int i = 0; i < n; i++)
        temp.pc[i] = pc[i]; // 0 到 n - 1
    for (int i = n; i < temp.n; i++)
        temp.pc[i] = f.pc[i - n]; // f.pc 从索引[0] 开始，对应PC[i] 索引从n开始, 索引为n 到 (n + f.n -1)前面部分为原对象元素；

    return temp;
}

Useless & Useless::operator=(const Useless & f)
{
    cout << "拷贝赋值operator=函数被调用！" << endl;
    if (this == &f)
        return *this;
    delete [] pc;
    n = f.n;
    pc = new char[n];
    for (int i = 0; i < n; i++)
        pc[i] = f.pc[i]; 
    return *this;
}

Useless & Useless::operator=(Useless && f)
{
    cout << "移动赋值函数被调用！" << endl;
    if (this == & f)
        return *this;
    delete [] pc;
    n = f.n;
    pc = f.pc;
    f.pc = nullptr;
    f.n = 0;
    return *this;
}

void Useless::ShowObject() const
{
    cout << "对象数量为：" << n << endl;
    cout << "数据地址为：" << (void*) pc << endl;
}

void Useless::ShowData() const
{
    if (n == 0)
        cout << "对象为空。" << endl;
    else
        for ( int i = 0; i < n; i++)
            cout << pc[i];
    cout << endl;
}

int main()
{
    cout << "Main Start!" << endl;
    {
        Useless one(10, 'x');
        Useless two = one + one;
        cout << "对象 one: " << endl;
        one.ShowData();
        cout << "对象 two: " << endl;
        two.ShowData();
        Useless three, four;
        cout << "three = one\n";
        three = one;
        cout << "当前对象 three = ";
        three.ShowData();
        cout << "且当前对象 one = ";
        one.ShowData();
        cout << "four = one + two\n";
        four = one + two;
        cout << "当前对象 four = ";
        four.ShowData();
        cout << "four = move(one)\n";
        four = std::move(one);
        cout << "当前对象 four = ";
        four.ShowData();
        cout << "且对象 one = ";
        one.ShowData();
    }
}