// strnbad.cpp
#include <cstring>
#include "string1.h"
using std::cin;
using std::cout;

// initilizing static memeber 
int String::num_strings = 0;

// class methods

// static method, 只能使用静态成员

int String::HowMany()
{
    return num_strings;
}

// cstr 
String::String(const char* s)
{
    len = std::strlen(s);
    str = new char[len + 1];
    std::strcpy(str, s);
    num_strings++;
    cout << num_strings << ": \"" << str << " \" object created\n";
}

String::String()
{
    len = 4;
    str = new char[1];
    str[0] = '\0';
    // str = strcpy(str, "C++");
    num_strings++;
    // cout << num_strings << ": \"" << str << "\" default object created\n";
}

String::String(const String & st) // copy constructor
{
    num_strings++;
    len = st.len;
    str = new char[len + 1];
    std::strcpy(str, st.str);
}

String::~String()
{
    cout << "\"" << str << "\" object deleted, ";
    --num_strings;
    cout << num_strings << " left\n";
    delete[] str;
}

//overloaded operator methods
String & String::operator=(const String & st)
{
    if (this == &st)
        return *this;
    delete[] str;
    len = st.len;
    str = new char[len + 1];
    std::strcpy(str, st.str);
    return *this;
}

String & String::operator=(const char * s)
{
    delete[] str;
    len = std::strlen(s);
    str = new char[len + 1];
    std::strcpy(str, s);
    return *this;
}
// 读写版本
char & String::operator[](int i)
{
    return str[i];
}

// 只读版本
const char & String::operator[](int i) const
{
    return str[i];
}

bool operator<(const String &st, const String &st2)
{
    return (std::strcmp(st.str, st2.str) < 0);
}

bool operator>(const String &st1, const String &st2)
{
    return st2 < st1;
}

bool operator==(const String &st, const String &st2)
{
    return (std::strcmp(st.str, st2.str) == 0);
}

// overloaded operator friend function
std::ostream& operator<<(std::ostream &os, const String & st)
{
    os << st.str;
    return os;
}

std::istream & operator>>(std::istream &is, String &st)
{
    char temp[String::CINLIM];
    is.get(temp, String::CINLIM);
    if (is)
        st = temp;
    while (is && is.get() != '\n')
        continue;
    return is;
}