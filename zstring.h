#ifndef ZSTRING_H
#define ZSTRING_H

#include <string>
#include <sstream>
#include <cstring>
//#include "asar.h"
#include "zassoc.h"
#include <cstdlib>
#include <algorithm>
//#include <cmath>

namespace LibChaos {

class ZString;
//typedef AssocArray<ZString> AsArZ;
typedef ZAssoc<ZString, ZString> AsArZ;
typedef ZArray<ZString> ArZ;

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
    std::string &str();

    ZString(char*);
    char *c();

    ZString(const char*);
    const char *cc();

    ZString(char);

    static std::string ItoS(long int num, int base = 10);
    ZString(int);
    int tint();

    char &operator[](unsigned int);

    int size();
    int length();
    int count(std::string);

    char first();
    char last();

    void clear();
    bool isEmpty();

    bool startsWith(ZString test_str, bool ignore_whitespace = true);
    bool beginsWith(ZString test);
    bool endsWith(ZString test);

    static unsigned long findFirst(ZString, ZString);

    // Replace the first occurrence of <before> in <str> with <after>, up to <max> times
    ZString &replace(ZString before, ZString after, unsigned max = 1000);
    static ZString replace(ZString str, ZString before, ZString after, unsigned max = 1000);
    // Replace up to <max> occurences of <before> with <after>
    ZString &replaceEach(ZString before, ZString after, unsigned max = 1000);
    static ZString replaceEach(ZString str, ZString before, ZString after, unsigned max = 1000);

    ZString findFirstBetween(ZString, ZString);
    ZString replaceBetween(ZString start, ZString end, ZString after);
    ZString findFirstXmlTagCont(ZString tag);
    ZString replaceXmlTagCont(ZString tag, ZString after);
    ZString label(ZString label, ZString value, bool modify = true);
    ZString label(AsArZ, bool modify = true);
    ZString strip(char target, bool modify = true);
    ZString removeWhitespace();

    ZString substr(int, bool modify = true);
    ZString substr(int, int, bool modify = true);

    ZString invert(bool modify = true);
    ZString toLower(bool modify = true);
    ZString duplicate(unsigned iterate, bool modify = true);
    ZString popLast(bool modify = true);

    ArZ explode(char delim);
    ArZ strict_explode(char delim);
    ArZ explode();

    //ZString toJSON(AsArZ, bool modify = true);
    //bool validJSON();
    //AsArZ fromJSON();

    friend std::ostream &operator<<(std::ostream& lhs, ZString rhs);
private:
    std::string data;
    char byte;
};

} // namespace LibChaos

#endif // ZSTRING_H
