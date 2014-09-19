/*****************************************
**               LibChaos               **
**               zstring.h              **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZSTRING_H
#define ZSTRING_H

#include "zassoc.h"
#include "ztypes.h"

#include <string>

namespace LibChaos {
//namespace ZStringInternal {

template <typename chartype> class ZStringT {
public:
    ZStringT() : _size(0), _data(nullptr){}
    ZStringT(const chartype *ptr, zu64 size) : _size(size), _data(new chartype[_size]){
        memcpy(_data, ptr, strSize());
    }
    ZStringT(const ZStringT<chartype> &other) : ZStringT(other._data, other._size){}
    ZStringT(const ZArray<chartype> &array) : ZStringT(array.ptr(), array.size()){}

    ~ZStringT(){
        delete[] _data;
    }

    // Assignment
    ZStringT<chartype> &operator=(const ZStringT<chartype> &rhs){
        clear();
        _size = rhs._size;
        _data = new chartype[_size];
        memcpy(_data, rhs._data, strSize());
    }

    // Comparison
    template <typename chartype2>
    friend bool operator==(const ZStringT<chartype2> &lhs, const ZStringT<chartype2> &rhs);
    template <typename chartype2>
    friend bool operator!=(const ZStringT<chartype2> &lhs, const ZStringT<chartype2> &rhs);

    // Concatenates this and <str> and returns result
    ZStringT<chartype> concat(const ZStringT<chartype> &str) const {
        ZStringT<chartype> out = *this;
        out.append(str);
        return out;
    }
    inline ZStringT<chartype> operator+(const ZStringT<chartype> &str) const { return concat(str); }

    // Appends <str> to this and returns this
    ZStringT<chartype> &append(const ZStringT<chartype> &str){
        chartype *buff = new chartype[_size + str._size];
        memcpy(buff, _data, strSize());
        memcpy(buff + _size, str._data, str.strSize());
        _size += str._size;
        delete[] _data;
        _data = buff;
    }
    inline ZStringT<chartype> &operator+=(const ZStringT<chartype> &str){ return append(str); }
    inline ZStringT<chartype> &operator<<(const ZStringT<chartype> &str){ return append(str); }

    void clear(){
        _size = 0;
        delete[] _data;
        _data = nullptr;
    }

    zu64 size() const {
        return _size;
    }
    zu64 strSize() const {
        return _size * sizeof(chartype);
    }

    char first() const {
        if(_size < 1)
            throw "Invalid index";
        return _data[0];
    }
    char last() const {
        if(_size < 1)
            throw "Invalid index";
        return _data[_size-1];
    }

private:
    zu64 _size;
    chartype *_data;
};

template <typename chartype>
inline bool operator==(const ZStringT<chartype> &lhs, const ZStringT<chartype> &rhs){
    if(lhs.size() != rhs.size())
        return false;
    return memcmp(lhs._data, rhs._data, lhs.strSize());
}
template <typename chartype>
inline bool operator!=(const ZStringT<chartype> &lhs, const ZStringT<chartype> &rhs){
    return !operator==(lhs, rhs);
}

typedef ZStringT<char> ZString;

typedef ZArray<ZString> ArZ;
typedef ZAssoc<ZString, ZString> AsArZ;

template <>
class ZStringT<char> : public ZStringT<char> {
public:
    ZStringT(std::string);
    std::string &str();
    const std::string &str() const;

    ZStringT(std::wstring);
    std::wstring wstr() const;

    //ZString(char*);
    //char *c();

    ZStringT(const unsigned char *);
    ZStringT(const char *);
    ZStringT(const char *str, zu64 size);
    const char *cc() const;

#if PLATFORM == WINDOWS
    ZString(const wchar_t*);
    const wchar_t *wc() const;
#endif

    ZStringT(char, zu64 len = 1);

    static ZString ItoS(zu64 num, unsigned base = 10, zu64 pad = 0);
    static ZString ItoS(zs64 num, unsigned base = 10);
    ZStringT(zu16);
    ZStringT(zs16);
    ZStringT(zu32);
    ZStringT(zs32);
    ZStringT(zint);
    ZStringT(zuint);
    ZStringT(zu64);
    ZStringT(zs64);
    int tint() const;

    // Construct from double with <places> decimal points, 0 means all
    ZStringT(double flt, unsigned places = 0);

    char &operator[](zu64 i){
        return _data[i];
    }
    const char &operator[](zu64 i) const {
        return _data[i];
    }

    zu64 size() const;
    inline zu64 length() const { return size(); }
    zu64 count(ZString) const;

    void clear();
    bool isEmpty() const;

    // Tests if <str> begins with <test>. Ignores whitespace at beginning of string if <ignore_whitespace>
    bool startsWith(ZString test, bool ignore_whitespace = true) const;
    // Alias for startsWith, always ignores whitespace
    inline bool beginsWith(ZString test) const { return startsWith(test, false); }

    // Tests if <str> ends with <test>
    bool endsWith(ZString test) const;

    // Insert character at direction
    ZString &insert(zu64 pos, ZString txt);
    static ZString insert(ZString str, zu64 pos, ZString txt);

    // Get portion of <str> from <pos> to end
    ZString &substr(zu64 pos);
    static ZString substr(ZString str, zu64 pos);

    // Get <len> characters after <pos> of <str>
    ZString &substr(zu64 pos, zu64 len);
    static ZString substr(ZString str, zu64 pos, zu64 len);

    // Get location of first character of first occurrence of <find> in <str>
    zu64 findFirst(ZString find) const;
    static zu64 findFirst(ZString str, ZString find);

    // Get locations of first characters of all non-overlapping occurrences of <find> in <str>
    ZArray<zu64> findAll(ZString find) const;
    static ZArray<zu64> findAll(ZString str, ZString find);

    // Replace section <len> characters long at <pos> with <after> in <str>
    ZString &replace(zu64 pos, zu64 len, ZString after);
    static ZString replace(ZString str, zu64 pos, zu64 len, ZString after);

    // Replace the first occurrence of <before> in <str> with <after>, up to <max> times
    ZString &replaceRecursive(ZString before, ZString after, unsigned max = 1000);
    static ZString replaceRecursive(ZString str, ZString before, ZString after, unsigned max = 1000);

    // Replace up to <max> occurences of <before> with <after> in <str>
    // <max> = -1 for unlimited
    ZString &replace(ZString before, ZString after, unsigned max = 0);
    static ZString replace(ZString str, ZString before, ZString after, unsigned max = 0);

    // Get sub-string of <str> before first occurence of <find> in <str>
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

    ArZ split(ZString delim) const;

    ArZ explode(char delim) const;
    ArZ strExplode(ZString delim) const;
    ArZ quotedExplode(char delim) const;
    ArZ escapedExplode(char delim) const;
    ArZ explodeList(unsigned nargs, ...) const;
    //ArZ explode();

    static ZString compound(ArZ parts, ZString delim);

    bool isUtf8(ZString);

    //ZString toJSON(AsArZ, bool modify = true);
    //bool validJSON();
    //AsArZ fromJSON();

    //ZString format(ZString fmt_str, ...);
    //ZString &format(...);

    static bool alphaTest(ZString str1, ZString str2);

    // Allows ZString to be used with std streams
    friend std::ostream &operator<<(std::ostream& lhs, ZString rhs);
private:
    //std::string _data;

};

//} // namespace ZStringInternal

//typedef ZStringInternal::ZString<char> ZString;
//template <class T> using ZStringT = ZStringInternal::ZString<T>;

} // namespace LibChaos

#endif // ZSTRING_H
