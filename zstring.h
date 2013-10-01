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
#include "ztypes.h"

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

    ZString(zu16);
    ZString(zs16);
    ZString(zu32);
    ZString(zs32);
    ZString(zuint);
    ZString(zsint);
    ZString(zu64);
    ZString(zs64);
    static ZString ItoS(zu64 num, int base = 10);
    static ZString ItoS(zs64 num, int base = 10);
    int tint();

    char &operator[](zu64);

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

    // Get portion of <str> from <pos> to end
    ZString &substr(zu64 pos);
    static ZString substr(ZString str, zu64 pos);
    // Get <len> characters after <pos> of <str>
    ZString &substr(zu64 pos, zu64 len);
    static ZString substr(ZString str, zu64 pos, zu64 len);

    static unsigned long findFirst(ZString, ZString);

    ZString &replace(zu64 pos, zu64 len, ZString after);
    static ZString replace(ZString str, zu64 pos, zu64 len, ZString after);
    // Replace the first occurrence of <before> in <str> with <after>, up to <max> times
    ZString &replaceRecursive(ZString before, ZString after, unsigned max = 1000);
    static ZString replaceRecursive(ZString str, ZString before, ZString after, unsigned max = 1000);
    // Replace up to <max> occurences of <before> with <after>
    // <max> = -1 for unlimited
    ZString &replace(ZString before, ZString after, unsigned max = -1);
    static ZString replace(ZString str, ZString before, ZString after, unsigned max = -1);

    ZString findFirstBetween(ZString, ZString);
    ZString replaceBetween(ZString start, ZString end, ZString after);
    ZString findFirstXmlTagCont(ZString tag);
    ZString replaceXmlTagCont(ZString tag, ZString after);
    ZString label(ZString label, ZString value, bool modify = true);
    ZString label(AsArZ, bool modify = true);
    ZString strip(char target, bool modify = true);
    ZString removeWhitespace();

    ZString invert(bool modify = true);
    ZString toLower(bool modify = true);
    ZString duplicate(unsigned iterate, bool modify = true);
    ZString popLast();

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
