#ifndef ZSTRING_H
#define ZSTRING_H

#include "zassoc.h"
#include "ztypes.h"

#include <string>

//#if PLATFORM == WINDOWS && ! ZSTRING_NO_WINDOWS_H
//    #include <windows.h>
//    #define ZSTRING_WCHAR
//#endif

namespace LibChaos {

//#define ZSTRING_UNICODEs
//#ifdef ZSTRING_UNICODE
//    typedef std::wstring zstring_stl;
//    typedef wchar_t zstring_char;
//#else
//    typedef std::string zstring_stl;
//    typedef char zstring_char;
//#endif

class ZString;
//typedef AssocArray<ZString> AsArZ;
typedef ZArray<ZString> ArZ;
typedef ZAssoc<ZString, ZString> AsArZ;

class ZString {
public:
    ZString();
    //~ZString();

    ZString &operator=(ZString);
    bool operator==(ZString) const;
    bool operator!=(ZString) const;
    ZString concat(ZString str) const; // Concatenates this and <str> and returns result
    inline ZString operator+(ZString str) const { return concat(str); }
    ZString &append(ZString str); // Appends <str> to this
    inline ZString &operator+=(ZString str){ return append(str); }
    inline ZString &operator<<(ZString str){ return append(str); }

    ZString(std::string);
    std::string &str();

    ZString(std::wstring);
    std::wstring wstr() const;

    //ZString(char*);
    //char *c();

    ZString(const unsigned char *);
    ZString(const char *);
    const char *cc() const;

#if PLATFORM == WINDOWS
    ZString(const wchar_t*);
    const wchar_t *wc() const;
#endif

    ZString(char, zu64 len = 1);

    ZString(zu16);
    ZString(zs16);
    ZString(zu32);
    ZString(zs32);
    ZString(zint);
    ZString(zuint);
    //explicit ZString(zsint);
    ZString(zu64);
    ZString(zs64);

    static ZString ItoS(zu64 num, unsigned base = 10, zu64 pad = 0);
    static ZString ItoS(zs64 num, unsigned base = 10);
    int tint() const;

    ZString(double flt, unsigned places = -1);

    ZString(ZArray<char> bin);

    char &operator[](zu64);

    zu64 size() const;
    inline zu64 length() const { return size(); }
    zu64 count(ZString) const;

    char first() const;
    char last() const;

    void clear();
    bool isEmpty() const;

    bool startsWith(ZString test, bool ignore_whitespace = true) const;
    inline bool beginsWith(ZString test) const { return startsWith(test, false); }
    bool endsWith(ZString test) const;

    // Get portion of <str> from <pos> to end
    ZString &substr(zu64 pos);
    static ZString substr(ZString str, zu64 pos);

    // Get <len> characters after <pos> of <str>
    ZString &substr(zu64 pos, zu64 len);
    static ZString substr(ZString str, zu64 pos, zu64 len);

    // Get location of first character of first occurrence of <find> in <str>
    static zu64 findFirst(ZString str, ZString find);

    // Replace section <len> characters long at <pos> with <after> in <str>
    ZString &replace(zu64 pos, zu64 len, ZString after);
    static ZString replace(ZString str, zu64 pos, zu64 len, ZString after);

    // Replace the first occurrence of <before> in <str> with <after>, up to <max> times
    ZString &replaceRecursive(ZString before, ZString after, unsigned max = 1000);
    static ZString replaceRecursive(ZString str, ZString before, ZString after, unsigned max = 1000);

    // Replace up to <max> occurences of <before> with <after> in <str>
    // <max> = -1 for unlimited
    ZString &replace(ZString before, ZString after, unsigned max = -1);
    static ZString replace(ZString str, ZString before, ZString after, unsigned max = -1);

    static ZString getUntil(ZString str, ZString find);

    ZString findFirstBetween(ZString, ZString);
    ZString replaceBetween(ZString start, ZString end, ZString after);
    ZString findFirstXmlTagCont(ZString tag);
    ZString replaceXmlTagCont(ZString tag, ZString after);
    ZString label(ZString label, ZString value, bool modify = true);
    ZString label(AsArZ, bool modify = true);

    // Strip occurences of <target> from beginning and end of <str>
    ZString &strip(char target);
    static ZString strip(ZString str, char target);

    ZString removeWhitespace();

    ZString invert(bool modify = true);

    // Convert UPPERCASE characters to lowercase equivalents in <str>
    ZString &toLower();
    static ZString toLower(ZString str);

    ZString duplicate(unsigned iterate, bool modify = true);
    ZString popLast();
    static ZString compound(ArZ parts, ZString delim);

    ArZ split(ZString delim);

    ArZ explode(char delim);
    ArZ explodeList(unsigned nargs, ...);
    ArZ strict_explode(char delim);
    ArZ explode();

    bool isUtf8(ZString);

    //ZString toJSON(AsArZ, bool modify = true);
    //bool validJSON();
    //AsArZ fromJSON();

    //ZString format(ZString fmt_str, ...);
    //ZString &format(...);

    static bool alphaTest(ZString str1, ZString str2);

    friend std::ostream &operator<<(std::ostream& lhs, ZString rhs);
private:
    std::string data;
    char byte;
};

} // namespace LibChaos

#endif // ZSTRING_H
