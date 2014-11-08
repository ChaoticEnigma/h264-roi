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

//#include "zstorage.h"
//#include "zdefaultstorage.h"

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

typedef ZArray<ZString> ArZ;
typedef ZAssoc<ZString, ZString> AsArZ;

// Represents UTF-8 string
// Wide characters are narrowed and encoded in UTF-8
// Internal array is always null terminated
class ZString {
public:
    typedef char chartype;

    enum {
        none = (zu64)-1
    };

public:
    ZString() : _size(0), _realsize(0), _data(nullptr){
        clear(); // Empty string with null terminator
    }
    ~ZString(){
        _alloc.dealloc(_data);
    }

//    ZString(ZStorage *stor) : ZString(){
//        if(stor != nullptr){
//            resize(stor->size());
//            stor->copyBlockTo(0, stor->size(), (zbyte *)_data);
//        }
//    }

    // Assumed UTF-8
    ZString(const chartype *ptr, zu64 size) : _size(0), _realsize(0), _data(nullptr){
        resize(size);
        if(size && ptr)
            _alloc.rawcopy(ptr, _data, size);
    }
    ZString(const ZString &other) : _size(0), _realsize(0), _data(nullptr){
        resize(other._size);
        if(other._size && other._data)
            _alloc.rawcopy(other._data, _data, other._size);
    }

    // char ZArray constructor
    ZString(const ZArray<chartype> &array) : _size(0), _realsize(0), _data(nullptr){
        resize(array.size());
        if(array.size() && array.ptr())
            _alloc.rawcopy(array.ptr(), _data, array.size());
    }

    // std strings
    // Assumed UTF-8
    ZString(std::string str);
    std::string str() const;

    // std wide strings
    // Assumed UCS-2 and converted to UTF-8
    ZString(std::wstring wstr);
    std::wstring wstr() const;

    // C-string constructor
    // Assumed UTF-8
    ZString(const chartype *str) : ZString(){
        if(str != nullptr){
            zu64 i = 0;
            while(str[i] != 0){
                ++i;
            }
            operator=(ZString(str, i));
        }
    }

    // Pointer to data
    inline const chartype *cc() const {
        return _data;
    }
    inline chartype *c() const {
        return _data;
    }

    ZString(const wchar_t *wstr);

    // Fill constructor
    ZString(chartype ch, zu64 len = 1) : _size(0), _realsize(0), _data(nullptr){
        resize(len);
        if(len)
            for(zu64 i = 0; i < len; ++i)
                _data[i] = ch;
    }

    // Interger to string
    static ZString ItoS(zu64 num, unsigned base = 10, zu64 pad = 0);
    static ZString ItoS(zs64 num, unsigned base = 10);

    ZString(zuc num) : ZString((zull)num){}
    ZString(zsc num) : ZString((zsll)num){}
    ZString(zus num) : ZString((zull)num){}
    ZString(zss num) : ZString((zsll)num){}
    ZString(zuint num) : ZString((zull)num){}
    ZString(zint num) : ZString((zsll)num){}
//#if COMPILER == MINGW
    ZString(zul num) : ZString((zull)num){}
    ZString(zsl num) : ZString((zsll)num){}
//#endif

    ZString(zull num) : ZString(){
        operator=(ItoS((zu64)num, 10));
    }
    ZString(zsll num) : ZString(){
        operator=(ItoS((zs64)num, 10));
    }

    // String to int
    int tint() const;

    // Construct from double with <places> decimal points, 0 means all
    ZString(double flt, unsigned places = 0);

    // Assignment
    ZString &assign(const ZString &other){
        resize(other.size());
        if(other.size())
            _alloc.rawcopy(other._data, _data, other.size());
        return *this;
    }
    inline ZString &operator=(const ZString &rhs){
        return assign(rhs);
    }

    // Comparison
    friend bool operator==(const ZString &lhs, const ZString &rhs);
    friend bool operator!=(const ZString &lhs, const ZString &rhs);

    // Clear
    inline ZString &clear(){
        resize(0);
        return *this;
    }

    // Concatenates this and <str> and returns result
    ZString concat(const ZString &str) const {
        ZString out = *this;
        out.append(str);
        return out;
    }
    friend ZString operator+(const ZString &lhs, const ZString &rhs);

    // Appends <str> to this and returns this
    ZString &append(const ZString &str){
        if(str.size()){
            zu64 oldsize = size();
            resize(oldsize + str.size());
            _alloc.rawcopy(str._data, _data + oldsize, str.size());
        }
        return *this;
    }
    inline ZString &operator+=(const ZString &str){ return append(str); }
    inline ZString &operator<<(const ZString &str){ return append(str); }

    inline char &operator[](zu64 i){
        return _data[i];
    }
    inline const char &operator[](zu64 i) const {
        return _data[i];
    }

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

    ZString &invert();
    static ZString invert(ZString str);

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

    bool isUtf8(ZString);

    //ZString format(ZString fmt_str, ...);
    //ZString &format(...);

    static bool charIsAlphabetic(chartype ch);

    static bool alphaTest(ZString str1, ZString str2);

    // Allows ZString to be used with std streams
    friend std::ostream &operator<<(std::ostream &lhs, ZString rhs);

    bool isEmpty() const {
        return size() == 0;
    }

    // Number of bytes (code units)
    inline zu64 size() const { return _size; }
    inline zu64 realSize() const { return _realsize; }

    // Number of *characters* (code points)
    zu64 length() const;

    chartype &first(){
        return _data[0];
    }
    const chartype &first() const {
        return _data[0];
    }
    chartype &last(){
        return _data[size() - 1];
    }
    const chartype &last() const {
        return _data[size() - 1];
    }

private:
    // Resize (IMPORTANT: memory is only allocated and initialized here)
    void resize(zu64 len);
    static bool _charIsWhitespace(chartype ch);
    zu64 _strReplace(const ZString &before, const ZString &after, zu64 startpos);

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

} // namespace LibChaos

#endif // ZSTRING_H
