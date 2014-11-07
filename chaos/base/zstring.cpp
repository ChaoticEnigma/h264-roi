#include "zstring.h"

#include <sstream>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cstdarg>
#include <clocale>
#include <locale>

#include <string>

#include <nowide/convert.hpp>

namespace LibChaos {

ZString::ZString(std::string str) : ZString(str.c_str(), str.size()){}
std::string ZString::str() const {
    return std::string(_data, size());
}

ZString::ZString(std::wstring wstr) : ZString(nowide::narrow(wstr)){}
std::wstring ZString::wstr() const {
    return nowide::widen(_data);
}

ZString::ZString(const wchar_t *wstr) : ZString(nowide::narrow(wstr)){}

ZString ZString::ItoS(zu64 value, unsigned base, zu64 pad){
    std::string buf;
    if(base < 2 || base > 16)
        return buf;
    buf.reserve(35);
    zu64 quotient = value;
    do {
        buf += "0123456789abcdef"[std::labs((long)((zu64)quotient % (zu64)base))];
        quotient /= base;
    } while(quotient);
    std::reverse(buf.begin(), buf.end());
    ZString tmp = buf;
    if(tmp.size() < pad){
        tmp = ZString('0', pad-tmp.size()) + tmp;
    }
    return tmp;
}
ZString ZString::ItoS(zs64 value, unsigned base){
    std::string buf;
    if (base < 2 || base > 16)
        return buf;
    buf.reserve(35);
    zs64 quotient = value;
    do {
        buf += "0123456789abcdef"[std::labs(quotient % (zs64)base)];
        quotient /= base;
    } while(quotient);
    if (value < 0) buf += '-';
    std::reverse(buf.begin(), buf.end());
    return ZString(buf);
}

int ZString::tint() const {
    return atoi(cc());
}

ZString::ZString(double num, unsigned places) : ZString(){
    std::ostringstream stream;
    stream << num;
    if(places){
        ArZ arr = ZString(stream.str()).split('.');
        assign(arr[0] + '.' + arr[1].substr(0, places));
    } else {
        assign(stream.str());
    }
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
    return test == substr(size() - test.size(), test.size());
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

ZString &ZString::invert(){
    ZString buff;
    for(zu64 i = 0, j = size(); i < size(); ++i, --j){
        buff += _data[j];
    }
    operator=(buff);
    return *this;
}
ZString ZString::invert(ZString str){
    return str.invert();
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

zu64 ZString::length() const {
    // This is wrong, WIP
    return size();
}

// ///////////////////////////////////////////////////////////////////////////////
// Private functions
// ///////////////////////////////////////////////////////////////////////////////

void ZString::resize(zu64 len){
    if(len > _realsize || _data == nullptr){ // Only reallocate if new size is larger than buffer
        chartype *buff = _alloc.alloc(len + 1); // New size + null terminator
        _alloc.rawcopy(_data, buff, _size); // Copy data to new buffer
        _realsize = len; // Update new buffer size
        _alloc.dealloc(_data); // Delete old buffer
        _data = buff;
    }
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
