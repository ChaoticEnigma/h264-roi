/*****************************************
**               LibChaos               **
**               zstring.h              **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZSTRING_H
#define ZSTRING_H

#include "zassoc.h"
#include "ztypes.h"

#include "zstorage.h"
#include "zdefaultstorage.h"

#include <cstring>

//#include <bits/stringfwd.h>
#include <string>
#include <iosfwd>

namespace LibChaos {

class ZString;

typedef ZArray<ZString> ArZ;
typedef ZAssoc<ZString, ZString> AsArZ;

class ZString {
public:
    typedef char chartype;

    enum {
        none = (zu64)-1
    };

public:
    ZString() : _size(0), _data(nullptr){
        resize(0);
    }
    ZString(ZStorage *stor) : ZString(){
        if(stor != nullptr){
            resize(stor->size());
            stor->copyBlockTo(0, stor->size(), (zbyte *)_data);
        }
    }

    ZString(const chartype *ptr, zu64 size) : ZString(){
        if(size && ptr){
            resize(size);
            _copy(_data, ptr, size);
        }
    }
    ZString(const ZString &other) : ZString(other._data, other.size()){}

    // char ZArray constructor
    ZString(const ZArray<chartype> &array) : ZString(array.ptr(), array.size()){}

    // std::string constructor
    ZString(std::string);
    std::string str() const;

    // C-string constructor
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

//    ZString(std::wstring);
//    std::wstring wstr() const;
//    ZString(const wchar_t*);
//    const wchar_t *wc() const;

    // Fill constructor
    ZString(chartype ch, zu64 len = 1) : ZString(){
        if(len){
            resize(len);
            for(zu64 i = 0; i < len; ++i){
                _data[i] = ch;
            }
        }
    }

    // Interger to string
    static ZString ItoS(zu64 num, unsigned base = 10, zu64 pad = 0);
    static ZString ItoS(zs64 num, unsigned base = 10);

    ZString(zu64 num) : ZString(){
        operator=(ItoS(num, 10));
    }
    ZString(zs64 num) : ZString(){
        operator=(ItoS(num, 10));
    }

    ZString(zuc num) : ZString((zu64)num){}
    ZString(zsc num) : ZString((zs64)num){}
    ZString(zus num) : ZString((zu64)num){}
    ZString(zss num) : ZString((zs64)num){}
    //ZString(zul num) : ZString((zu64)num){}
    //ZString(zsl num) : ZString((zs64)num){}
    //ZString(zull num) : ZString((zu64)num){}
    //ZString(zsll num) : ZString((zs64)num){}

    ZString(zuint num) : ZString((zu64)num){}
    ZString(zint num) : ZString((zs64)num){}

//    explicit ZString(zu8 num) : ZString((zu64)num){}
//    explicit ZString(zs8 num) : ZString((zs64)num){}
//    explicit ZString(zu16 num) : ZString((zu64)num){}
//    explicit ZString(zs16 num) : ZString((zs64)num){}
//    explicit ZString(zu32 num) : ZString((zu64)num){}
//    explicit ZString(zs32 num) : ZString((zs64)num){}

    // String to int
    int tint() const;

    // Construct from double with <places> decimal points, 0 means all
    ZString(double flt, unsigned places = 0);

    // Destructor
    ~ZString(){
        delete[] _data;
    }

    // Assignment
    ZString &assign(const ZString &other){
        resize(other.size());
        _copy(_data, other._data, other.size());
        return *this;
    }
    inline ZString &operator=(const ZString &rhs){
        return assign(rhs);
    }

    // Comparison
    friend bool operator==(const ZString &lhs, const ZString &rhs);
    friend bool operator!=(const ZString &lhs, const ZString &rhs);

    // Resize (IMPORTANT: memory is only allocated and initialized here)
    // _data must always have null terminator
    ZString &resize(zu64 len){
        chartype *buff = new chartype[len + 1];
        _copy(buff, _data, MIN(len, _size));
        buff[len] = 0;
        _size = len;
        delete[] _data;
        _data = buff;
        return *this;
    }

    // Clear
    ZString &clear(){
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
            _copy(_data + oldsize, str._data, str.size());
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

    ArZ explode(chartype delim) const;
    ArZ strExplode(ZString delim) const;
    ArZ quotedExplode(chartype delim) const;
    ArZ escapedExplode(chartype delim) const;
    ArZ explodeList(unsigned nargs, ...) const;
    //ArZ explode();

    static ZString compound(ArZ parts, ZString delim);

    bool isUtf8(ZString);

    //ZString format(ZString fmt_str, ...);
    //ZString &format(...);

    static bool alphaTest(ZString str1, ZString str2);

    // Allows ZString to be used with std streams
    friend std::ostream &operator<<(std::ostream &lhs, ZString rhs);

    bool isEmpty() const {
        return size() == 0;
    }

    zu64 size() const {
        return _size;
    }
    inline zu64 length() const { return size(); }

    zu8 charSize() const {
        return sizeof(chartype);
    }
    zu64 strSize() const {
        return size() * charSize();
    }

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
    static void _copy(chartype *dest, const chartype *src, zu64 size);
    static bool _charIsWhitespace(chartype ch);

    zu64 _strReplace(const ZString &before, const ZString &after, zu64 startpos);

private:
    zu64 _size;
    chartype *_data;

};

inline ZString operator+(const ZString &lhs, const ZString &rhs){
    return lhs.concat(rhs);
}

inline bool operator==(const ZString &lhs, const ZString &rhs){
    if(lhs.size() != rhs.size())
        return false;
    return memcmp(lhs._data, rhs._data, lhs.strSize()) == 0;
}
inline bool operator!=(const ZString &lhs, const ZString &rhs){
    return !operator==(lhs, rhs);
}

} // namespace LibChaos

#endif // ZSTRING_H
