#include "zstring.h"

#include <sstream>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cstdarg>
#include <clocale>
#include <locale>

namespace LibChaos {

ZString::ZString() : _data(){

}
ZString::ZString(const ZString &other) : _data(other._data){

}

//ZString::~ZString(){
//    //data.clear();
//}

ZString &ZString::operator=(const ZString &str_){
    _data = str_.str();
    return *this;
}
ZString ZString::concat(ZString str_) const {
    std::string tmp = _data;
    tmp.append(str_.str());
    return ZString(tmp);
}
ZString &ZString::append(ZString str_){
    _data = _data.append(str_.str());
    return *this;
}

ZString::ZString(std::string str_) : _data(str_){}
std::string &ZString::str(){
    return _data;
}
const std::string &ZString::str() const {
    return _data;
}

ZString::ZString(std::wstring wide) : _data(wide.begin(), wide.end()){}
std::wstring ZString::wstr() const {
    return std::wstring(_data.begin(), _data.end());
}

//ZString::ZString(char *str_){
//    if(str_ != NULL){
//        data = std::string(str_, strlen(str_));
//    } else {
//        data = std::string();
//    }
//}
//char *ZString::c(){
//    char str_[size()];
//    return strcpy(str_, data.c_str());
//}

ZString::ZString(const unsigned char *str_) : _data(){
    if(str_ != NULL){
        _data = std::string((const char *)str_, strlen((const char *)str_));
    }
}
ZString::ZString(const char *str_) : _data(){
    if(str_ != NULL){
        _data = std::string(str_, strlen(str_));
    }
}
const char *ZString::cc() const {
    return _data.c_str();
}

#if PLATFORM == WINDOWS
}
#include <windows.h>
namespace LibChaos {

ZString::ZString(const wchar_t *wstr){
    if(wstr != NULL){
        int len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
        char *str = new char[len];
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
        data = std::string(str, len);
    }
}
const wchar_t *ZString::wc() const {
    int len = MultiByteToWideChar(CP_UTF8, 0, cc(), size(), NULL, 0);
    wchar_t *wstr = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, cc(), size(), wstr, len);
    return wstr;
}
#endif

ZString::ZString(char ch, zu64 len){
    _data = std::string(len, ch);
}

ZString::ZString(zu16 num){ _data = ItoS((zu64)num, 10).str(); }
ZString::ZString(zs16 num){ _data = ItoS((zs64)num, 10).str(); }
ZString::ZString(zu32 num){ _data = ItoS((zu64)num, 10).str(); }
ZString::ZString(zs32 num){ _data = ItoS((zs64)num, 10).str(); }
ZString::ZString(zint num){ _data = ItoS((zs64)num, 10).str(); }
ZString::ZString(zuint num){ _data = ItoS((zu64)num, 10).str(); }
//ZString::ZString(zsint num){ data = ItoS((zs64)num, 10).str(); }
ZString::ZString(zu64 num){ _data = ItoS(num, 10).str(); }
ZString::ZString(zs64 num){ _data = ItoS(num, 10).str(); }

ZString ZString::ItoS(zu64 value, unsigned base, zu64 pad){
    std::string buf;
    if(base < 2 || base > 16)
        return buf;
    buf.reserve(35);
    zu64 quotient = value;
    do {
        buf += "0123456789abcdef"[std::labs(quotient % (zu64)base)];
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
    const char *str_ = _data.c_str();
    return atoi(str_);
}

ZString::ZString(double num, unsigned places){
    std::ostringstream stream;
    stream << num;
    if(places){
        ArZ arr = ZString(stream.str()).explode('.');
        _data = (arr[0] + '.' + arr[1].substr(0, places)).str();
    } else {
        _data = stream.str();
    }
}

ZString::ZString(ZArray<char> bin){
    for(zu64 i = 0; i < bin.size(); ++i)
        append(bin[i]);
}

zu64 ZString::size() const {
    return _data.size();
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

char ZString::first() const {
    if(size() >= 1)
        return _data[0];
    else
        return '\0';
}
char ZString::last() const {
    if(size() >= 1)
        return _data[size()-1];
    else
        return '\0';
}

void ZString::clear(){
    _data.clear();
}
bool ZString::isEmpty() const {
    return _data.empty();
}

bool ZString::startsWith(ZString test, bool ignorews) const {
    ZString start = test;
    bool started;
    if(ignorews)
        started = false;
    else
        started = true;
    unsigned j = 0;
    for(unsigned i = 0; i < _data.size(); ++i){
        if(_data[i] == start[j]){
            if(j == test.size()-1)
                return true;
            started = true;
            ++j;
        } else if(_data[i] == ' ' || _data[i] == '\t'){
            if(started)
                return false;
        } else {
            return false;
        }
    }
    return true;
}

bool ZString::endsWith(ZString test) const {
    if(test.size() > size())
        return false;
    ZString end = _data.substr(_data.size() - test.size(), test.size());
    return test == end;
}

ZString &ZString::insert(zu64 pos, ZString txt){
    _data.insert(pos, txt.str());
    return *this;
}
ZString ZString::insert(ZString str, zu64 pos, ZString txt){
    return str.insert(pos, txt);
}

ZString &ZString::substr(zu64 pos){
    _data = substr(pos, std::string::npos).str();
    return *this;
}
ZString ZString::substr(ZString str, zu64 pos){
    return str.substr(pos);
}

ZString &ZString::substr(zu64 pos, zu64 len){
    _data = _data.substr(pos, len);
    return *this;
}
ZString ZString::substr(ZString str, zu64 pos, zu64 len){
    if(str.size() < pos || !len)
        return ZString();
    return str.substr(pos, len);
}

zu64 ZString::findFirst(ZString str, ZString find){
    return str.str().find(find.str());
}

ZString &ZString::replace(zu64 pos, zu64 len, ZString after){
    _data = replace(_data, pos, len, after).str();
    return *this;
}
ZString ZString::replace(ZString str, zu64 pos, zu64 len, ZString after){
    return substr(str, 0, pos) + after + substr(str, pos+len);
}

ZString &ZString::replaceRecursive(ZString before, ZString after, unsigned max){
    _data = replace(_data, before, after, max).str();
    return *this;
}
ZString ZString::replaceRecursive(ZString str, ZString before, ZString after, unsigned max){
    if(str.isEmpty())
        return ZString();
    if(before.isEmpty())
        return str;
    bool found = true;
    unsigned count = 0;
    while(found && count < max){
        unsigned long loc = findFirst(str, before);
        if(loc != (unsigned long)-1){
            str.str().replace(loc, before.size(), after.str());
            ++count;
        } else {
            found = false;
        }
    }
    return str;
}

ZString &ZString::replace(ZString before, ZString after, unsigned max){
    _data = replace(_data, before, after, max).str();
    return *this;
}
ZString ZString::replace(ZString str, ZString before, ZString after, unsigned max){
    if(str.isEmpty())
        return ZString();
    if(before.isEmpty())
        return str;
    bool unlim = false;
    if(max == 0)
        unlim = true;

    bool found = true;
    unsigned count = 0;
    unsigned last = 0;
    while(found && (count < max || unlim)){
        //unsigned long loc = findFirst(str, before);
        unsigned long loc = str.str().find(before.str(), last);
        if(loc != std::string::npos){
            str.str().replace(loc, before.size(), after.str());
            last = loc + after.size();
            ++count;
        } else {
            found = false;
        }
    }
    return str;
}

ZString ZString::getUntil(ZString str, ZString find){
    zu64 loc = findFirst(str, find);
    return str.substr(0, loc);
}

ZString ZString::findFirstBetween(ZString opening_string, ZString closing_string){
    std::string pre = opening_string.str();
    std::string post = closing_string.str();
    if(pre.size() <= 0 || post.size() <= 0)
        return ZString();
    std::string tmp = _data;
    ZString found;
    enum pos_type {
        outside, sopen, eopen, inside, sclose, eclose
    } pos = outside;
    unsigned j = 0;
    unsigned jl = 0;
    unsigned k = 0;
    unsigned kl = 0;
    ZString tmpbuff;
    for(unsigned i = 0; i < tmp.size(); ++i){
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
    _data = tmp.str();
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

ZString ZString::label(AsArZ values, bool modify){
    for(unsigned i = 0; i < values.size(); ++i)
        label(values.key(i), values[i], modify);
    return ZString(_data);
}
ZString ZString::label(ZString labeltxt, ZString value, bool modify){
    ZString txt = ZString("<?").append(labeltxt).append("?>");
    if(modify)
        return replace(txt, value);
    else
        return replace(_data, txt, value);
}

ArZ ZString::split(ZString delim){
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

/*
ArZ ZString::quotedExplode(char delim) const {
    ArZ out;
    std::string str_ = data;
    for(zu64 i = 0; i < str_.length(); ++i){
        if(str_[i] == '"'){
            for(zu64 j = i; j < str_.length(); ++j){
                if(str_[j] == '"'){
                    out.push(str_.substr(0, j));
                    str_ = str_.substr(j+1, str_.length());
                    i = (zu64)-1;
                }
            }
        } else if(str_[i] == delim){
            out.push(str_.substr(0, i));
            str_ = str_.substr(i+1, str_.length());
            i = (zu64)-1;
        }
    }
    out.push(str_);
    return out;
}
*/

ArZ ZString::strict_explode(char delim){
    ArZ out;
    std::string str_ = _data;
    for(unsigned i = 0; i < str_.length(); ++i){
        if(str_[i] == delim && str_[i-1] != '\\'){
            std::string sub = str_.substr(0, i);
            if(sub != ""){
                out.push(sub);
            }
            str_ = str_.substr(i+1, str_.length());
            i = (zu64)-1;
        }
    }
    out.push(str_);
    return out;
}

#ifndef BUILDING
    #define VAARGTYPE NULL
#else
    #define VAARGTYPE int
#endif
ArZ ZString::explodeList(unsigned nargs, ...){
    va_list args;
    va_start(args, nargs);
    ZArray<char> delims;
    for(unsigned i = 0; i < nargs; ++i){
        delims.push(va_arg(args, VAARGTYPE));
    }
    va_end(args);

    ArZ out;
    std::string str_ = _data;
    for(zu64 i = 0; i < str_.length(); ++i){
        bool br = false;
        for(zu64 j = 0; j < delims.size(); ++j){
            if(delims[j] == str_[i]){
                br = true;
                break;
            }
        }
        if(br){
            out.push(str_.substr(0, i));
            str_ = str_.substr(i+1, str_.length());
            i = (zu64)-1;
        }
    }
    out.push(str_);
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

ZString &ZString::strip(char target){
    _data = strip(_data, target).str();
    return *this;
}
ZString ZString::strip(ZString str, char target){
    zu64 clen = 0;
    for(zu64 i = 0; i < str.size(); ++i){
        if(str[i] == target)
            ++clen;
        else
            break;
    }
    if(clen > 0)
        str.substr(clen, str.size());

    clen = 0;
    for(zu64 i = 0; i < str.size(); ++i){
        zu64 curr = str.size() - 1 - i;
        if(str[curr] == target)
            ++clen;
        else
            break;
    }
    if(clen > 0)
        str.substr(0, str.size() - clen);

    return str;
}

ZString ZString::removeWhitespace(){
    replace("  ", "");
    replace("\n", "");
    replace("\r", "");
    return *this;
}

ZString ZString::invert(bool modify){
    std::string buff;
    for(unsigned i = _data.length(); i > 0; --i){
        buff += _data[i];
    }

    if(modify){
        _data = buff;
        return ZString(_data);
    } else {
        return ZString(buff);
    }
}

ZString &ZString::toLower(){
    _data = toLower(_data).str();
    return *this;
}
ZString ZString::toLower(ZString str){
    for(zu64 i = 0; i < str.size(); ++i){
        // Custom tolower()
        //if((int)tmp[i] >= 65 && (int)tmp[i] <= 90)
        //    tmp[i] = (char)((int)tmp[i] + 32);
        str[i] = tolower(str[i]);
    }
    return str;
}

ZString ZString::duplicate(unsigned iter, bool modify){
    ZString tmp;
    for(unsigned i = 0; i < iter; ++i){
        tmp << _data;
    }
    if(modify){
        _data = tmp.str();
        return ZString(_data);
    } else {
        return tmp;
    }
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

ZString ZString::popLast(){
    return substr(0, size()-1);
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

// ///////////////////////////////////////////////////////////////////////////////
// Non-member functions
// ///////////////////////////////////////////////////////////////////////////////

std::ostream &operator<<(std::ostream& lhs, ZString rhs){
    lhs << rhs.str();
    return lhs;
}

}
