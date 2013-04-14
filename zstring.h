#ifndef ZSTRING_H
#define ZSTRING_H

#include <string>
#include <cstdio>
#include "zarray.h"
#include <cstdlib>
#include <cstring>

typedef

class ZString {
public:
    ZString() : data(){}
    //ZString(const ZString &str) : data(){for(unsigned long i = 0; i < str.size(); ++i){data.push_back(str.ZAc()[i]);}}
    ~ZString(){}

    ZString &operator=(ZString);
    bool operator==(ZString);
    bool operator!=(ZString);
    ZString operator+(ZString);
    ZString &append(ZString);
    ZString &operator+=(ZString str){ return append(str); }
    ZString &operator<<(ZString str){ return append(str); }

    ZString(ZArray<char> arr) : data(){ data = arr; }
    ZArray<char> ZAc(){ return data; }

    ZString(std::string str) : data(str.c_str(), str.size()){}
    ZString &operator=(const std::string str){ return operator=(ZString(str)); }
    bool operator==(const std::string str){ return operator==(ZString(str)); }
    bool operator!=(const std::string str){ return operator!=(ZString(str)); }
    ZString operator+(const std::string str){ return operator+(ZString(str)); }
    ZString &operator+=(const std::string str){ return operator+=(ZString(str)); }
    ZString &operator<<(const std::string str){ return operator<<(ZString(str)); }
    std::string str(){ return std::string(cc()); }

    ZString(char *str) : data(str, strlen(str)){}
    ZString &operator=(char *str){ return operator=(ZString(str)); }
    bool operator==(char *str){ return operator==(ZString(str)); }
    bool operator!=(char *str){ return operator!=(ZString(str)); }
    ZString operator+(char *str){ return operator+(ZString(str)); }
    ZString &operator+=(char *str){ return operator+=(ZString(str)); }
    ZString &operator<<(char *str){ return operator<<(ZString(str)); }
    char *c(){ ZArray<char> tmp = ZAc(); tmp.push_back(NULL); return tmp.c_style(); }

    ZString(const char *str) : data(str, strlen(str)){}
    ZString &operator=(const char *str){ return operator=(ZString(str)); }
    bool operator==(const char *str){ return operator==(ZString(str)); }
    bool operator!=(const char *str){ return operator!=(ZString(str)); }
    ZString operator+(const char *str){ return operator+(ZString(str)); }
    ZString &operator+=(const char *str){ return operator+=(ZString(str)); }
    ZString &operator<<(const char *str){ return operator<<(ZString(str)); }
    const char *cc(){ return (const char *)c(); }

    ZString(char ch) : data(){ data.push_back(ch); }
    ZString &operator=(char str){ return operator=(ZString(str)); }
    ZString operator+(char str){ return operator+(ZString(str)); }
    ZString &operator+=(char str){ return operator+=(ZString(str)); }
    ZString &operator<<(char str){ return operator<<(ZString(str)); }

    ZString(int);
    int tint();

    char &operator[](unsigned long num){ return data[num]; }

    unsigned long size(){ return data.size(); }
    unsigned long length(){ return size(); }

    int count(ZString);

    struct SubZStr {
        unsigned long start;
        unsigned long end;
    };

    ZString substr(int, int, bool modify = true);
    ZString substrpos(int, int, bool modify = true);
    ZArray<SubZStr> findAll(ZString target);
    ZString replace(ZString before, ZString after, bool modify = true);
    ZString label(std::string label, ZString value, bool modify = true);
    ZString strip(char target, bool modify = true);

    ZString invert(bool modify = true);
    ZString toLower(bool modify = true);

    friend std::ostream &operator<<(std::ostream& lhs, ZString rhs);
private:
    ZArray<char> data;
};

#endif // ZSTRING_H
