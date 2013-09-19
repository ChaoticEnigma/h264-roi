#ifndef ZSTRING_H
#define ZSTRING_H

#include <string>
#include <sstream>
#include <cstring>
#include "asar.h"
#include <cstdlib>
#include <algorithm>
//#include <cmath>

namespace LibChaos {

class ZString;
typedef AssocArray<ZString> AsArZ;

class ZString {
public:
    ZString();
    ~ZString();

    ZString &operator=(ZString);
    bool operator==(ZString);
    bool operator!=(ZString);
    ZString concat(ZString);
    ZString operator+(ZString);
    ZString &append(ZString);
    ZString &operator+=(ZString);
    ZString &operator<<(ZString);

    ZString(std::string);
//    ZString &operator=(std::string);
//    bool operator==(const std::string);
//    bool operator!=(const std::string);
//    ZString operator+(std::string);
//    ZString &operator+=(std::string);
//    ZString &operator<<(std::string);
    std::string &str();

    ZString(char*);
//    ZString &operator=(char*);
//    bool operator==(char*);
//    bool operator!=(char*);
//    ZString operator+(char*);
//    ZString &operator+=(char*);
//    ZString &operator<<(char*);
    char *c();

    ZString(const char*);
//    ZString &operator=(const char*);
//    bool operator==(const char*);
//    bool operator!=(const char*);
//    ZString operator+(const char*);
//    ZString &operator+=(const char*);
//    ZString &operator<<(const char*);
    const char *cc();

    ZString(char);
//    ZString &operator=(char);
//    ZString operator+(char);
//    ZString &operator+=(char);
//    ZString &operator<<(char);

    static std::string ItoS(long int num, int base = 10);
    ZString(int);
    int tint();

    char &operator[](unsigned int index){return data[index]; }

    int size();
    int length();
    int count(std::string);

    char first(){ return data[0]; }
    char last(){ return data[size()-1]; }

    void clear();
    bool isEmpty();

    bool startsWith(ZString test_str, bool ignore_whitespace = true);
    bool beginsWith(ZString test);
    bool endsWith(ZString test);

    ZString replace(ZString before, ZString after, bool modify = true);
    ZString findFirstBetween(ZString, ZString);
    ZString replaceBetween(ZString start, ZString end, ZString after);
    ZString findFirstXmlTagCont(ZString tag);
    ZString replaceXmlTagCont(ZString tag, ZString after);
    ZString label(std::string label, ZString value, bool modify = true);
    ZString label(AsArZ, bool modify = true);
    ZString strip(char target, bool modify = true);
    ZString removeWhitespace();
    ZString substr(int, bool modify = true);
    ZString substr(int, int, bool modify = true);
    ZString invert(bool modify = true);
    ZString toLower(bool modify = true);
    ZString duplicate(unsigned iterate, bool modify = true);
    ZString popLast(bool modify = true);

    AsArZ explode(char delim);
    AsArZ strict_explode(char delim);
    AsArZ explode();

    ZString toJSON(AsArZ, bool modify = true);
    bool validJSON();
    AsArZ fromJSON();

    friend std::ostream &operator<<(std::ostream& lhs, ZString rhs);
private:
    std::string data;
};

} // namespace LibChaos

#endif // ZSTRING_H
