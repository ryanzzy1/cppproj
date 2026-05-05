// dma.cpp

#include "dma.h"
#include <cstring>

// baseDMA methods

baseDMA::baseDMA(const char * l, int r)
{
    lable = new char[std::strlen(l) + 1];
    std::strcpy(lable, l);
    rating = r;
}

baseDMA::baseDMA(const baseDMA & rs)
{
    lable = new char[std::strlen(rs.lable) + 1];
    std::strcpy(lable, rs.lable);
    rating = rs.rating;
}

baseDMA& baseDMA::operator=(const baseDMA & rs)
{
    if (this == &rs)
        return *this;
    delete [] lable;
    lable = new char[std::strlen(rs.lable) + 1];
    std::strcpy(lable, rs.lable);
    rating = rs.rating;
    return *this;
}

baseDMA::~baseDMA()
{
    delete [] lable;
}

std::ostream & operator<<(std::ostream & os, const baseDMA & rs)
{
    os << "Lable: " << rs.lable << "\n";
    os << "Rating: " << rs.rating << "\n";
    return os;
}

// lacksDMA methods
lacksDMA::lacksDMA(const char * c, const char * l, int r)
    : baseDMA(l, r)
{
    std::strncpy(color, c, COL_LEN -1);
    color[COL_LEN -1] = '\0'; // color[39] = '\0';
}

lacksDMA::lacksDMA(const char * c, const baseDMA & rs)
    : baseDMA(rs)
{
    std::strncpy(color, c, COL_LEN -1);
    color[COL_LEN -1] = '\0'; // color[39] = '\0';
}

std::ostream & operator<<(std::ostream & os, const lacksDMA & ls)
{
    os << (const baseDMA &) ls;
    os << "Color: " << ls.color << std::endl;
    return os;
}

// hasDMA methods
hasDMA::hasDMA(const char * s, const char * l, int r)
    : baseDMA(l, r)
{
    style = new char[std::strlen(s) + 1];
    std::strcpy(style, s);
}

hasDMA::hasDMA(const char * s, const baseDMA & rs)
    : baseDMA(rs)
{
    style = new char[std::strlen(s) + 1];
    std::strcpy(style, s);
}

hasDMA::hasDMA(const hasDMA & hs)
    : baseDMA(hs)
{
    style = new char[std::strlen(hs.style) + 1];
    std::strcpy(style, hs.style);
}

hasDMA::~hasDMA()
{
    delete [] style;
}

hasDMA & hasDMA::operator=(const hasDMA & rs)
{
    if (this == &rs)
        return *this;
    baseDMA::operator=(rs);
    delete [] style;
    style = new char[std::strlen(rs.style) + 1];
    std::strcpy(style, rs.style);
    return *this;
}

std::ostream & operator<<(std::ostream & os, const hasDMA & hs)
{
    os << (const baseDMA &) hs;
    os << "Style: " << hs.style << std::endl;
    return os;
}