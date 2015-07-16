/*******************************************************************************
**                                  LibChaos                                  **
**                                 zstring.cpp                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zstring.h"
#include "zarray.h"
#include "zmath.h"

// std::stringstream
#include <sstream>
// std::reverse
#include <algorithm>
// Variable arguments lists
#include <cstdarg>

#if PLATFORM == MACOSX
    #include <math.h>
#endif

namespace LibChaos {

ZString::ZString() : _size(0), _realsize(0), _data(nullptr){
    clear(); // Empty string with null terminator
}

ZString::~ZString(){
    if(_data != nullptr)
        _alloc.dealloc(_data);
}

ZString::ZString(const ZString &other) : ZString(){
    resize(other._size);
    if(other._size && other._data)
        _alloc.rawcopy(other._data, _data, other._size);
}

ZString::ZString(const ZString::chartype *str) : ZString(){
    if(str != nullptr){
        zu64 i = 0;
        while(str[i] != 0){
            ++i;
        }
        ZString filled(str, i);
        swap(filled);
    }
}

ZString::ZString(const ZString::chartype *ptr, zu64 length) : ZString(){
    resize(length);
    if(length && ptr)
        _alloc.rawcopy(ptr, _data, length);
}

ZString::ZString(const ZArray<ZString::chartype> &array) : ZString(array.raw(), array.size()){

}

ZString::ZString(std::string str) : ZString(str.c_str(), str.size()){}

std::string ZString::str() const {
    return std::string(_data, size());
}

ZString::ZString(const wchar_t *wstr) : ZString(){
    fromUtf16(wstr);
}

ZString::ZString(const wchar_t *wstr, zu64 length) : ZString(){
    fromUtf16(std::wstring(wstr, length));
}

ZString::ZString(const ZArray<wchar_t> &array) : ZString(array.raw(), array.size()){

}

ZString::ZString(std::wstring wstr) : ZString(){
    fromUtf16(wstr);
}

std::wstring ZString::wstr() const {
    return toUtf16();
}

ZString::ZString(ZString::chartype ch, zu64 len) : ZString(){
    resize(len);
    if(len)
        for(zu64 i = 0; i < len; ++i)
            _data[i] = ch;
}

ZString ZString::ItoS(zu64 value, zu8 base, zu64 pad){
    ZString buffer;
    if(base < 2 || base > 16)
        return buffer;
    buffer.reserve(35);
    zu64 quotient = value;
    do {
        buffer += "0123456789abcdef"[ZMath::abs(quotient % (zu64)base)];
        quotient /= base;
    } while(quotient);
    buffer.reverse();
    if(buffer.size() < pad)
        buffer.append(ZString('0', pad - buffer.size()));
    return buffer;
}

ZString ZString::ItoS(zs64 value, zu8 base){
    return (value < 0 ? "-" : "") + ItoS((zu64)ZMath::abs(value), base);
}

bool ZString::isInteger(zu8 base) const {
    if(isEmpty())
        return false;
    if(base > 16) // Only supports up to hexadecimal
        return false;
    const char *digits = "0123456789abcdef";
    for(zu64 i = 0; i < size(); ++i){
        bool yes = false;
        for(zu8 j = 0; j < base; ++j){
            if(tolower(operator[](i)) == digits[j])
                yes = true;
        }
        if(!yes)
            return false;
    }
    return true;
}

int ZString::tint() const {
    return atoi(cc());
}

zu64 ZString::tozu64(zu8 base) const {
    if(!isInteger(base))
        return ZU64_MAX;
    ZString tmp = reverse(*this);
    zu64 out = 0;
    for(zu64 i = 0; i < tmp.size(); ++i){
        char ch = tolower(tmp[i]);
        char digit = (ch < 58 ? (ch - 48) : (ch - 97 + 10));
        out += ((zu64)(digit) * (zu64)pow(base, i));
    }
    return out;
}

bool ZString::isFloat() const {
    if(isEmpty())
        return false;
    ZString tmp = *this;
    if(tmp[0] == '-')
        tmp.substr(1);
    ArZ parts = tmp.explode('.');
    if(parts.size() < 2)
        return tmp.isInteger();
    if(parts.size() > 2)
        return false;
    if(!parts[0].isInteger() || !parts[1].isInteger())
        return false;
    return true;
}

float ZString::toFloat() const {
    if(!isFloat())
        return 0.0f;
    float out = 0.0f;
    ZString tmp = *this;
    bool neg = false;
    if(tmp[0] == '-'){
        neg = true;
        tmp.substr(1);
    }
    ArZ parts = split('.');
    ZString whole = reverse(parts[0]);
    for(zu64 i = 0; i < whole.size(); ++i){
        out += ((float)(whole[i] - 48) * pow(10, i));
    }
    if(parts.size() > 1){
        for(zu64 i = 0; i < parts[1].size(); ++i){
            out += ((float)(parts[1][i] - 48) * pow(10, -(zs64)i));
        }
    }
    if(neg)
        out = -out;
    return out;
}

ZString::ZString(double num, unsigned places) : ZString(){
    std::stringstream stream;
    stream << num;
    if(places){
        ArZ arr = ZString(stream.str()).split('.');
        assign(arr[0] + '.' + arr[1].substr(0, places));
    } else {
        assign(stream.str());
    }
}

ZString &ZString::assign(const ZString &other){
    resize(other.size());
    if(other.size())
        _alloc.rawcopy(other._data, _data, other.size());
    return *this;
}

void ZString::reserve(zu64 size){
    if(size > _realsize || _data == nullptr){ // Only reallocate if new size is larger than buffer
        // TEST: newsize, but always leave extra space for null terminator, but don't count null terminator in realsize
        zu64 newsize = MAX(_realsize * 2, size);
        chartype *buff = _alloc.alloc(newsize + 1); // New size + null terminator
        _alloc.rawcopy(_data, buff, _size); // Copy data to new buffer
        // Update new buffer size
        _realsize = newsize;
        _alloc.dealloc(_data); // Delete old buffer
        _data = buff;
    }
}

ZString &ZString::append(const ZString &str){
    if(str.size()){
        zu64 oldsize = size();
        resize(oldsize + str.size());
        _alloc.rawcopy(str._data, _data + oldsize, str.size());
    }
    return *this;
}

ZString ZString::concat(const ZString &str) const {
    ZString out = *this;
    out.append(str);
    return out;
}

ZString &ZString::prepend(const ZString &str){
    if(str.size()){
        zu64 oldsize = size();
        resize(str.size() + oldsize);
        _alloc.rawmove(_data, _data + str.size(), oldsize);
        _alloc.rawcopy(str._data, _data, str.size());
    }
    return *this;
}

zu64 ZString::count(ZString needle) const {
    ZString haystack = _data;
    zu64 cnt = 0;
    for(zu64 i = 0; i < haystack.size(); ++i){
        if(haystack[i] == needle[0]){
            bool good = true;
            for(zu64 g = i, j = 0; j < needle.size(); ++g, ++j){
                if(haystack[g] != needle[j]){
                    good = false;
                    break;
                }
            }
            if(good)
                ++cnt;
        }
    }
    return cnt;
}

bool ZString::startsWith(const ZString &test, bool ignorews) const {
    bool started;
    if(ignorews)
        started = false;
    else
        started = true;
    unsigned j = 0;
    for(zu64 i = 0; i < size(); ++i){
        if(_data[i] == test[j]){
            if(j == test.size()-1)
                return true;
            started = true;
            ++j;
        } else if(_charIsWhitespace(_data[i])){
            if(started)
                return false;
        } else {
            return false;
        }
    }
    return true;
}

bool ZString::startsWith(const ZString &str, const ZString &test, bool ignorews){
    return str.startsWith(test, ignorews);
}

bool ZString::endsWith(ZString test) const {
    if(test.size() > size())
        return false;
    ZString end = substr(*this, size() - test.size(), test.size());
    return test == end;
}

ZString &ZString::insert(zu64 pos, const ZString &txt){
    if(txt.size()){
        pos = MIN(pos, size());
        ZString after = substr(*this, pos);
        resize(pos);
        append(txt).append(after);
    }
    return *this;
}

ZString ZString::insert(ZString str, zu64 pos, const ZString &txt){
    return str.insert(pos, txt);
}

ZString &ZString::substr(zu64 pos){
    substr(pos, none);
    return *this;
}

ZString ZString::substr(ZString str, zu64 pos){
    return str.substr(pos);
}

ZString &ZString::substr(zu64 pos, zu64 len){
    if(pos < size()){
        len = MIN(len, size() - pos);
        assign(ZString(_data + pos, len));
    } else {
        clear();
    }
    return *this;
}

ZString ZString::substr(ZString str, zu64 pos, zu64 len){
    if(pos >= str.size() || len == 0)
        return ZString();
    return str.substr(pos, len);
}

zu64 ZString::findFirst(const ZString &find, zu64 start) const {
    if(find.size() && find.size() <= size() && start < size()){
        zu64 startpos = 0;
        zu64 j = 0;
        for(zu64 i = start; i < size(); ++i){
            if(_data[i] == find[j]){
                if(j == 0)
                    startpos = i;
                ++j;
                if(j == find.size())
                    return startpos;
            } else if(j != 0){
                --i;
                j = 0;
            }
        }
    }
    return none;
}

zu64 ZString::findFirst(const ZString &str, const ZString &find, zu64 start){
    return str.findFirst(find, start);
}

ZArray<zu64> ZString::findAll(const ZString &findstr) const {
    ZArray<zu64> finds;
    zu64 next = 0;
    do {
        zu64 pos = findFirst(findstr, next);
        if(pos == std::string::npos){
            break;
        }
        finds.push(pos);
        next = pos + findstr.size();
    } while(next < size());
    return finds;
}

ZArray<zu64> ZString::findAll(const ZString &str, const ZString &find){
    return str.findAll(find);
}

ZString &ZString::replacePos(zu64 pos, zu64 len, const ZString &after){
    len = MIN(len, size() - pos);
    ZString part2 = substr(*this, pos + len);
    resize(pos);
    append(after).append(part2);
    return *this;
}

ZString ZString::replacePos(ZString str, zu64 pos, zu64 len, const ZString &after){
    return str.replacePos(pos, len, after);
}

ZString &ZString::replaceFirst(const ZString &before, const ZString &after, zu64 start){
    if(before.size() > size() || before == after)
        return *this;
    _strReplace(before, after, start);
    return *this;
}

ZString ZString::replaceFirst(ZString str, const ZString &before, const ZString &after, zu64 start){
    return str.replaceFirst(before, after, start);
}

ZString &ZString::replace(const ZString &before, const ZString &after, zu64 max){
    if(before.size() > size() || before == after)
        return *this;

    bool unlim = max == 0 ? true : false;
    zu64 count = 0;
    zu64 last = 0;
    while(unlim || count < max){
        last = _strReplace(before, after, last);
        if(last == none)
            break;
        ++count;
    }
    return *this;
}

ZString ZString::replace(ZString str, const ZString &before, const ZString &after, zu64 max){
    return str.replace(before, after, max);
}

ZString &ZString::replaceRecursive(const ZString &before, const ZString &after, zu64 max){
    if(before.size() > size() || before == after)
        return *this;

    bool unlim = max == 0 ? true : false;
    zu64 count = 0;
    while(unlim || count < max){
        if(_strReplace(before, after, 0) == none)
            break;
        ++count;
    }
    return *this;
}

ZString ZString::replaceRecursive(ZString str, const ZString &before, const ZString &after, zu64 max){
    return str.replaceRecursive(before, after, max);
}

ZString ZString::getUntil(ZString str, const ZString &find){
    zu64 loc = findFirst(str, find);
    return str.substr(0, loc);
}

ZString ZString::findFirstBetween(ZString pre, ZString post){
    if(pre.size() <= 0 || post.size() <= 0)
        return ZString();
    ZString tmp = *this;
    ZString found;
    enum pos_type {
        outside, sopen, eopen, inside, sclose, eclose
    };
    pos_type pos = outside;
    zu64 j = 0;
    zu64 jl = 0;
    zu64 k = 0;
    zu64 kl = 0;
    ZString tmpbuff;
    for(zu64 i = 0; i < tmp.size(); ++i){
        char c = tmp[i];
        switch(pos){
        case outside:
            if(c == pre[pre.size()-1] && jl == pre.size()-1){
                pos = eopen;
            } else if(c == pre[0]){
                j = 1;
                jl = 1;
                pos = sopen;
            }
            break;
        case sopen:
            if(c == pre[pre.size()-1] && jl == pre.size()-1){
                pos = eopen;
            } else if(c == pre[j]){
                ++j;
                ++jl;
                pos = sopen;
            } else if(c == pre[0]){
                j = 1;
                jl = 1;
                pos = sopen;
            } else {
                j = 0;
                jl = 0;
                pos = outside;
            }
            break;
        case eopen:
            if(c == post[post.size()-1] && kl == post.size()-1){
                return found;
            } else if(c == post[0]){
                tmpbuff << c;
                k = 1;
                kl = 1;
                pos = sclose;
            } else {
                found << c;
            }
            break;
        case inside:
            break;
        case sclose:
            if(c == post[post.size()-1] && kl == post.size()-1){
                return found;
            } else if(c == post[k]){
                tmpbuff << c;
                ++k;
                ++kl;
                pos = sclose;
            } else if(c == post[0]){
                found << tmpbuff;
                tmpbuff.clear();
                tmpbuff << c;
                k = 1;
                kl = 1;
                pos = sclose;
            } else {
                found << tmpbuff;
                tmpbuff.clear();
                found << c;
                k = 0;
                kl = 0;
                pos = eopen;
            }
            break;
        case eclose:
            break;
        default:
            // Bad, Probable Memory Corruption
            break;
        }
    }
    found << tmpbuff;
    return found;
}

ZString ZString::replaceBetween(ZString opening_string, ZString closing_string, ZString after_string){
    ZString tmp = _data;
    bool done = false;
    while(!done){
        ZString inside = tmp.findFirstBetween(opening_string, closing_string);
        if(inside.isEmpty()){
            done = true;
            break;
        }
        ZString old = opening_string;
        old << inside << closing_string;
        tmp.replace(old, after_string);
    }
    operator=(tmp);
    return tmp;
}

ZString ZString::findFirstXmlTagCont(ZString tag){
    ZString open;
    open << "<" << tag << ">";
    ZString close;
    close << "</" << tag << ">";
    return findFirstBetween(open, close);
}

ZString ZString::replaceXmlTagCont(ZString tag, ZString after){
    ZString open;
    open << "<" << tag << ">";
    ZString close;
    close << "</" << tag << ">";
    return replaceBetween(open, close, after);
}

ZString &ZString::label(const ZString &labeltxt, const ZString &value){
    ZString txt = "<?" + labeltxt + "?>";
    replace(txt, value);
    return *this;
}

ZString &ZString::label(const AsArZ &values){
    for(zu64 i = 0; i < values.size(); ++i)
        label(values.key(i), values[i]);
    return *this;
}

ArZ ZString::split(ZString delim) const {
    ArZ out;
    zu64 pos = findFirst(*this, delim);
    if(pos == none){
        out.push(*this);
        return out;
    }
    out.push(substr(_data, 0, pos));
    out.push(substr(_data, pos + delim.size()));
    return out;
}

ArZ ZString::explode(char delim) const {
    ArZ out;
    zu64 counter = 0;
    for(zu64 i = 0; i < size(); ++i){
        if(operator[](i) == delim){
            if(counter){
                out.push(substr(*this, i - counter, counter));
                counter = 0;
            }
            continue;
        }
        ++counter;
    }
    if(counter)
        out.push(substr(*this, size() - counter, counter));
    return out;
}

ArZ ZString::strExplode(const ZString &delim) const {
    ZArray<zu64> pos = findAll(delim);
    ArZ out;
    zu64 startpos = 0;
    for(zu64 i = 0; i < pos.size(); ++i){
        if(startpos < pos[i])
            out.push(substr(*this, startpos, pos[i] - startpos));
        startpos = pos[i] + delim.size();
    }
    if(startpos < size())
        out.push(substr(*this, startpos));
    return out;
}

ArZ ZString::quotedExplode(char delim) const {
    ArZ out;
    zu64 counter = 0;
    bool inquotes = false;
    for(zu64 i = 0; i < size(); ++i){
        if(operator[](i) == '\"'){
            if(counter)
                out.push(substr(*this, i - counter, counter));
            inquotes = !inquotes;
            counter = 0;
            continue;
        } else if(!inquotes && operator[](i) == delim){
            if(counter){
                out.push(substr(*this, i - counter, counter));
                counter = 0;
            }
            continue;
        }
        ++counter;
    }
    if(counter)
        out.push(substr(*this, size() - counter, counter));
    return out;
}

ArZ ZString::escapedExplode(char delim) const {
    ArZ out;
    zu64 counter = 0;
    for(zu64 i = 0; i < size(); ++i){
        if(operator[](i) == delim && i > 0 && operator[](i - 1) != '\\'){
            if(counter){
                out.push(substr(*this, i - counter, counter));
                counter = 0;
            }
            continue;
        }
        ++counter;
    }
    if(counter)
        out.push(substr(*this, size() - counter, counter));
    return out;
}

// Fixes syntax highlighting
#ifndef BUILDING
    #define VAARGTYPE NULL
#else
    #define VAARGTYPE int
#endif
ArZ ZString::explodeList(unsigned nargs, ...) const {
    va_list args;
    va_start(args, nargs);
    ZArray<char> delims;
    for(unsigned i = 0; i < nargs; ++i){
        delims.push(va_arg(args, VAARGTYPE));
    }
    va_end(args);

    ArZ out;
    zu64 counter = 0;
    for(zu64 i = 0; i < size(); ++i){
        // Test for any delimiter
        bool delimfound = false;
        for(zu64 j = 0; j < delims.size(); ++j){
            if(operator[](i) == delims[j]){
                delimfound = true;
                break;
            }
        }
        if(delimfound){
            if(counter){
                out.push(substr(*this, i - counter, counter));
                counter = 0;
            }
            continue;
        }
        ++counter;
    }
    if(counter)
        out.push(substr(*this, size() - counter, counter));
    return out;
}
#undef VAARGTYPE

ZString ZString::compound(ArZ parts, ZString delim){
    ZString name;
    for(zu64 i = 0; i < parts.size(); ++i){
        name += parts[i];
        if(i < parts.size() - 1)
            name += delim;
    }
    return name;
}

ZString &ZString::strip(chartype target){
    zu64 clen = 0;
    for(zu64 i = 0; i < size(); ++i){
        if(_data[i] == target)
            ++clen;
        else
            break;
    }
    if(clen > 0)
        substr(clen, size());

    clen = 0;
    for(zu64 i = 0; i < size(); ++i){
        zu64 curr = size() - 1 - i;
        if(_data[curr] == target)
            ++clen;
        else
            break;
    }
    if(clen > 0)
        substr(0, size() - clen);
    return *this;
}

ZString ZString::strip(ZString str, chartype target){
    return str.strip(target);
}

ZString ZString::removeWhitespace(){
    replace("  ", "");
    replace("\n", "");
    replace("\r", "");
    return *this;
}

ZString &ZString::reverse(){
    ZString buff;
    buff.reserve(size());
    for(zu64 i = size(); i > 0; --i){
        buff += _data[i-1];
    }
    swap(buff);
    return *this;
}

ZString ZString::reverse(ZString str){
    return str.reverse();
}

ZString &ZString::toLower(){
    for(zu64 i = 0; i < size(); ++i){
        // Custom tolower()
        //if((int)tmp[i] >= 65 && (int)tmp[i] <= 90)
        //    tmp[i] = (char)((int)tmp[i] + 32);
        _data[i] = tolower(_data[i]);
    }
    return *this;
}

ZString ZString::toLower(ZString str){
    return str.toLower();
}

ZString &ZString::duplicate(zu64 iter){
    ZString tmp;
    for(zu64 i = 0; i < iter; ++i){
        tmp += _data;
    }
    operator=(tmp);
    return *this;
}

//ZString ZString::format(ZString fmt_str, ...){
//    std::string fmt = fmt_str.str();
//    int final_n, n = fmt.size() * 2; /* reserve 2 times as much as the length of the fmt_str */
//    std::string str;
//    std::unique_ptr<char[]> formatted;
//    va_list ap;
//    while(1) {
//        formatted.reset(new char[n]); /* wrap the plain char array into the unique_ptr */
//        strcpy(&formatted[0], fmt.c_str());
//        va_start(ap, fmt);
//        final_n = vsnprintf(&formatted[0], n, fmt.c_str(), ap);
//        va_end(ap);
//        if (final_n < 0 || final_n >= n)
//            n += abs(final_n - n + 1);
//        else
//            break;
//    }
//    return ZString(formatted.get());
//}

//ZString &ZString::format(...){
//    va_list arglist;
//    ZString fmt = data;
//    va_start(arglist, fmt);
//    data = format(fmt, arglist).str();
//    va_end(arglist);
//    return *this;
//}

bool ZString::charIsAlphabetic(chartype ch){
    return (ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122);
}

bool ZString::alphaTest(ZString str1, ZString str2){
    for(zu64 k = 0; k < str1.size() && k < str2.size(); ++k){
        if(str1[k] == str2[k])
            continue;
        if(str1[k] < str2[k])
            return true;
        return false;
    }
    if(str1.size() <= str2.size())
        return true;
    return false;
}

void ZString::swap(ZString &other){
    zu64 size = _size;
    zu64 realsize = _realsize;
    chartype *data = _data;
    _size = other._size;
    _realsize = other._realsize;
    _data = other._data;
    other._size = size;
    other._realsize = realsize;
    other._data = data;
}

zu64 ZString::length() const {
    // This is wrong, WIP
    return size();
}

// ///////////////////////////////////////////////////////////////////////////////
// Private functions
// ///////////////////////////////////////////////////////////////////////////////

void ZString::resize(zu64 len){
    reserve(len);
    // If the new size is smaller, just change size
    _size = len;
    _data[_size] = 0; // Always null terminate
}

bool ZString::_charIsWhitespace(chartype ch){
    return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
}

zu64 ZString::_strReplace(const ZString &before, const ZString &after, zu64 startpos){
    zu64 pos = findFirst(before, startpos);
    if(pos != none)
        replacePos(pos, before.size(), after);
    return pos;
}

// ///////////////////////////////////////////////////////////////////////////////
// Friend functions
// ///////////////////////////////////////////////////////////////////////////////

std::ostream &operator<<(std::ostream& lhs, ZString rhs){
    lhs << rhs.str();
    return lhs;
}

}
