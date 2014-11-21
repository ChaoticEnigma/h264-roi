/*****************************************
**               LibChaos               **
**               zstring.h              **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZSTRING_H
#define ZSTRING_H

#include "ztypes.h"
#include "zallocator.h"
#include "zassoc.h"
#include "ziterator.h"

#include <cstring>

//#include <bits/stringfwd.h>
#include <string>
#include <iosfwd>

#if PLATFORM == WINDOWS
    #define UNICODE
    #define _UNICODE
#endif

namespace LibChaos {

class ZString;
class ZStringIterator;

typedef ZArray<ZString> ArZ;
typedef ZAssoc<ZString, ZString> AsArZ;

// Represents UTF-8 string
// Wide characters are narrowed and encoded in UTF-8
// Internal array is always null terminated
class ZString {
public:
    typedef char chartype;

    enum {
        none = ZU64_MAX
    };

public:
    ZString();
    ~ZString();

//    ZString(ZStorage *stor) : ZString(){
//        if(stor != nullptr){
//            resize(stor->size());
//            stor->copyBlockTo(0, stor->size(), (zbyte *)_data);
//        }
//    }

    ZString(const ZString &other);

    // Assumed UTF-8
    ZString(const chartype *str); // Null-terminated C-string
    ZString(const chartype *ptr, zu64 length);
    ZString(const ZArray<chartype> &array); //  ZArray<char>

    // std strings
    // Assumed UTF-8
    ZString(std::string str);
    std::string str() const;

    // Null-terminated wide string
    // Assumed UTF-16
    ZString(const wchar_t *wstr);
    ZString(const wchar_t *wstr, zu64 length);
    ZString(const ZArray<wchar_t> &array); //  ZArray<wchar_t>

    // std wide strings
    // Assumed UTF-16 and converted to UTF-8
    ZString(std::wstring wstr);
    std::wstring wstr() const;

    // Pointer to data
    inline const chartype *cc() const { return _data; }
    inline chartype *c() const { return _data; }

    // Fill constructor
    ZString(chartype ch, zu64 len = 1);

    // Interger to string
    static ZString ItoS(zu64 num, zu8 base = 10, zu64 pad = 0);
    static ZString ItoS(zs64 num, zu8 base = 10);

    ZString(zuc num) : ZString((zull)num){}
    ZString(zsc num) : ZString((zsll)num){}
    ZString(zus num) : ZString((zull)num){}
    ZString(zss num) : ZString((zsll)num){}
    ZString(zuint num) : ZString((zull)num){}
    ZString(zint num) : ZString((zsll)num){}
    ZString(zul num) : ZString((zull)num){}
    ZString(zsl num) : ZString((zsll)num){}

    ZString(zull num) : ZString(ItoS((zu64)num, 10)){}
    ZString(zsll num) : ZString(ItoS((zs64)num, 10)){}

    // To integer
    bool isInteger() const;
    int tint() const;
    zu64 tozu64() const;

    // To floating point
    bool isFloat() const;
    float toFloat() const;

    // Iterators
    ZStringIterator begin();
    ZStringIterator end();

    // Construct from double with <places> decimal points, 0 means all
    ZString(double flt, unsigned places = 0);

    // Assignment
    ZString &assign(const ZString &other);
    inline ZString &operator=(const ZString &rhs){ return assign(rhs); }

    // Comparison
    friend bool operator==(const ZString &lhs, const ZString &rhs);
    friend bool operator!=(const ZString &lhs, const ZString &rhs);

    // Clear
    inline void clear(){ resize(0); }

    // Resize (IMPORTANT: memory is only allocated and initialized here)
    void reserve(zu64 size);

    // Appends <str> to this and returns this
    ZString &append(const ZString &str);
    inline ZString &operator+=(const ZString &str){ return append(str); }
    inline ZString &operator<<(const ZString &str){ return append(str); }

    // Concatenates this and <str> and returns result
    ZString concat(const ZString &str) const;
    friend ZString operator+(const ZString &lhs, const ZString &rhs);

    // Prepends <str> to this and returns this
    ZString &prepend(const ZString &str);

    inline char &operator[](zu64 i){ return _data[i]; }
    inline const char &operator[](zu64 i) const { return _data[i]; }

    zu64 count(ZString) const;

    // Tests if <str> begins with <test>. Ignores whitespace at beginning of string if <ignore_whitespace>
    bool startsWith(const ZString &test, bool ignore_whitespace = true) const;
    static bool startsWith(const ZString &str, const ZString &test, bool ignore_whitespace = true);

    // Alias for startsWith, always ignores whitespace
    inline bool beginsWith(ZString test) const { return startsWith(test, false); }

    // Tests if <str> ends with <test>
    bool endsWith(ZString test) const;

    // Insert string at position
    ZString &insert(zu64 pos, const ZString &txt);
    static ZString insert(ZString str, zu64 pos, const ZString &txt);

    // Get portion of <str> from <pos> to end
    ZString &substr(zu64 pos);
    static ZString substr(ZString str, zu64 pos);

    // Get <len> characters after <pos> of <str>
    ZString &substr(zu64 pos, zu64 len);
    static ZString substr(ZString str, zu64 pos, zu64 len);

    // Get location of first character of first occurrence of <find> in <str>
    zu64 findFirst(const ZString &find, zu64 start = 0) const;
    static zu64 findFirst(const ZString &str, const ZString &find, zu64 start = 0);

    // Get locations of first characters of all non-overlapping occurrences of <find> in <str>
    ZArray<zu64> findAll(const ZString &find) const;
    static ZArray<zu64> findAll(const ZString &str, const ZString &find);

    // Replace section <len> characters long at <pos> with <after> in <str>
    ZString &replacePos(zu64 pos, zu64 len, const ZString &after);
    static ZString replacePos(ZString str, zu64 pos, zu64 len, const ZString &after);

    ZString &replaceFirst(const ZString &before, const ZString &after, zu64 start = 0);
    static ZString replaceFirst(ZString str, const ZString &before, const ZString &after, zu64 start = 0);

    // Replace up to <max> occurences of <before> with <after> in <str>
    // <max> = 0 for unlimited
    ZString &replace(const ZString &before, const ZString &after, zu64 max = 0);
    static ZString replace(ZString str, const ZString &before, const ZString &after, zu64 max = 0);

    // Replace the first occurrence of <before> in <str> with <after>, up to <max> times
    // <max> = 0 for unlimited
    ZString &replaceRecursive(const ZString &before, const ZString &after, zu64 max = 1000);
    static ZString replaceRecursive(ZString str, const ZString &before, const ZString &after, zu64 max = 1000);

    // Get sub-string of <str> before first occurence of <find> in <str>
    static ZString getUntil(ZString str, const ZString &find);

    ZString findFirstBetween(ZString, ZString);
    ZString replaceBetween(ZString start, ZString end, ZString after);
    ZString findFirstXmlTagCont(ZString tag);
    ZString replaceXmlTagCont(ZString tag, ZString after);

    ZString &label(const ZString &label, const ZString &value);
    ZString &label(const AsArZ &values);

    // Strip occurences of <target> from beginning and end of <str>
    ZString &strip(chartype target);
    static ZString strip(ZString str, chartype target);

    ZString removeWhitespace();

    ZString &reverse();
    static ZString reverse(ZString str);

    // Convert UPPERCASE characters to lowercase equivalents in <str>
    ZString &toLower();
    static ZString toLower(ZString str);

    // What is this for...?
    ZString &duplicate(zu64 iterate);

    ArZ split(ZString delim) const;

    // Explode a string into any array of substrings
    // All explode functions will treat consecutive delimiters as one delimitier
    // Delimiters at the beginning or end of a string are discarded
    ArZ explode(chartype delim) const;
    ArZ strExplode(const ZString &delim) const;
    ArZ quotedExplode(chartype delim) const;
    ArZ escapedExplode(chartype delim) const;
    ArZ explodeList(unsigned nargs, ...) const;
    //ArZ explode();

    static ZString compound(ArZ parts, ZString delim);

    //ZString format(ZString fmt_str, ...);
    //ZString &format(...);

    static bool charIsAlphabetic(chartype ch);

    static bool alphaTest(ZString str1, ZString str2);

    // Allows ZString to be used with std streams
    friend std::ostream &operator<<(std::ostream &lhs, ZString rhs);

    inline bool isEmpty() const { return (size() == 0); }

    // Number of bytes (code units)
    inline zu64 size() const { return _size; }
    inline zu64 realSize() const { return _realsize; }

    // Number of *characters* (code points)
    zu64 length() const;

    // On empty string, will return null terminator
    inline chartype &first(){ return _data[0]; }
    inline const chartype &first() const { return _data[0]; }
    inline chartype &last(){ return _data[size() - 1]; }
    inline const chartype &last() const { return _data[size() - 1]; }

private:
    void resize(zu64 len);
    static bool _charIsWhitespace(chartype ch);
    zu64 _strReplace(const ZString &before, const ZString &after, zu64 startpos);

    // Unicode Encoding
    void fromUtf16(std::wstring wstr);
    std::wstring toUtf16() const;

    static bool isUtf8(ZString str);

private:
    ZAllocator<chartype> _alloc;
    zu64 _size;
    zu64 _realsize;
    chartype *_data;
};

inline ZString operator+(const ZString &lhs, const ZString &rhs){
    return lhs.concat(rhs);
}

inline bool operator==(const ZString &lhs, const ZString &rhs){
    if(lhs.size() != rhs.size())
        return false;
    return memcmp(lhs._data, rhs._data, lhs.size()) == 0;
}
inline bool operator!=(const ZString &lhs, const ZString &rhs){
    return !operator==(lhs, rhs);
}

class ZStringIterator : public ZIterator<ZString::chartype> {
public:
    ZStringIterator(ZString *str, zu64 pos = 0) : str(str), pos(pos){}

    bool atEnd() const {
        return pos >= str->size();
    }
    void advance(){
        if(!atEnd())
            ++pos;
    }
    void operator++(){ advance(); }

    bool atFront() const {
        return pos <= 0;
    }
    void devance(){
        if(!atFront())
            --pos;
    }
    void operator--(){ devance(); }

    ZString::chartype &operator*(){
        return str->operator[](pos);
    }

private:
    ZString *str;
    zu64 pos;
};

} // namespace LibChaos

#endif // ZSTRING_H
