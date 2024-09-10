// rtti2.cpp -- using dynamic_cast, typeid, and type_info

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <typeinfo>
using namespace std;

class Grand
{
private:
    int hold;
public:
    Grand(int h = 0) : hold(h) {}
    virtual void Speak() const {"I am a grand class!\n";}
    virtual int Value() const {return hold;}
};

class Superb : public Grand
{
public:
    Superb(int h = 0) : Grand(h) {}
    void Speak() const {cout << "I am a Superb class!\n";}
    virtual void Say() const {cout << "I hold the superb value of " << Value() << "!\n";}
};

class Magnificient : public Superb
{
private:
    char ch;
public:
    Magnificient(int h = 0, char cv = 'A') : Superb(h), ch(cv) { }
    void Speak() const {cout << "I am a magnicficient class!\n";}
    void Say() const {cout << "I hold the character " << ch << " and the integer "
                    << Value() << "!\n";}
};

Grand *GetOne();

int main()
{
    srand(time(0));
    Grand * pg;
    Superb * ps;
    for (int i = 0; i < 5; i++)
    {
        pg = GetOne();
        cout << "Now processing type " << typeid(*pg).name() << "\n";
        pg->Speak();
        if (ps = dynamic_cast<Superb*>(pg))
            ps->Say();
        if (typeid(Magnificient) == typeid(*pg))
            cout << "Yes, you're really magnificient.\n";
    }

    return 0;
}

Grand * GetOne()
{
    Grand * p;
    switch(rand() % 3)
    {
    case 0 : p = new Grand(rand() % 100);
        break;
    case 1 : p = new Superb(rand() % 100);
        break;
    case 2 : p = new Magnificient(rand() % 100, 'A' + rand() % 26);
        break;
    }
    return p;
}