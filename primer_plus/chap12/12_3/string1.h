// strngbad.h 

#ifndef STRNGBAD_H_
#define STRNGBAD_H_

#include <iostream>
using std::ostream;
using std::istream;

class String
{
private:
    char* str;
    int len;
    static int num_strings;
    static const int CINLIM = 80;

public:
// constructors and other methods
    String(const char* s);
    String();
    String(const String &);
    ~String();
    int length() const {return len;};
// overloaded operator methods 
    String & operator=(const String &);
    String & operator=(const char *);
    char & operator[](int i);
    const char & operator[](int i) const;

    //overloaded operator friend function
    friend bool operator<(const String &st, const String &st2);
    friend bool operator>(const String &st1, const String &st2);
    friend bool operator==(const String &st, const String &st2);
    friend std::istream & operator>>(std::istream &is, String &st);
    friend std::ostream & operator<< (std::ostream & os,
                        const String & st);
// static function
    static int HowMany();
};

#endif