#include "zstring.h"

namespace LibChaos {

ZString::ZString() : data(){
    //data.clear();
}
ZString::~ZString(){
    //data.clear();
}

ZString &ZString::operator=(ZString str){
    data = str.str();
    return *this;
}
bool ZString::operator==(ZString str){
    return data == str.str();
}
bool ZString::operator!=(ZString str){
    return data != str.str();
}
ZString ZString::concat(ZString str){
    std::string tmp = data;
    tmp.append(str.str());
    return ZString(tmp);
}
ZString ZString::operator+(ZString str){
    return concat(str);
}
ZString &ZString::append(ZString str){
    data = data.append(str.str());
    return *this;
}
ZString &ZString::operator+=(ZString str){
    return append(str);
}
ZString &ZString::operator<<(ZString str){
    return append(str);
}

ZString::ZString(std::string str){
    data = str;
}
std::string &ZString::str(){
    return data;
}

ZString::ZString(char *str){
    if(str != NULL){
        data = std::string(str, strlen(str));
    } else {
        data = std::string();
    }
}
char* ZString::c(){
    char str[size()];
    return strcpy(str, data.c_str());
}

ZString::ZString(const char *str){
    if(str != NULL){
        data = std::string(str, strlen(str));
    } else {
        data = std::string();
    }
}
const char* ZString::cc(){
    return data.c_str();
}

ZString::ZString(char ch){
    data = std::string(1, ch);
}

ZString::ZString(zu16 num){ data = ItoS((zu64)num, 10).str(); }
ZString::ZString(zs16 num){ data = ItoS((zs64)num, 10).str(); }
ZString::ZString(zu32 num){ data = ItoS((zu64)num, 10).str(); }
ZString::ZString(zs32 num){ data = ItoS((zs64)num, 10).str(); }
ZString::ZString(zuint num){ data = ItoS((zu64)num, 10).str(); }
ZString::ZString(zsint num){ data = ItoS((zs64)num, 10).str(); }
ZString::ZString(zu64 num){ data = ItoS((zu64)num, 10).str(); }
ZString::ZString(zs64 num){ data = ItoS((zs64)num, 10).str(); }

ZString ZString::ItoS(zu64 value, int base) {
    std::string buf;
    if (base < 2 || base > 16) return buf;
    enum { kMaxDigits = 35 };
    buf.reserve( kMaxDigits );
    zu64 quotient = value;
    do {
        buf += "0123456789abcdef"[std::labs(quotient % base)];
        quotient /= base;
    } while(quotient);
    if ( value < 0) buf += '-';
    std::reverse( buf.begin(), buf.end() );
    return ZString(buf);
}
ZString ZString::ItoS(zs64 value, int base) {
    std::string buf;
    if (base < 2 || base > 16) return buf;
    enum { kMaxDigits = 35 };
    buf.reserve( kMaxDigits );
    zs64 quotient = value;
    do {
        buf += "0123456789abcdef"[std::labs(quotient % base)];
        quotient /= base;
    } while(quotient);
    if ( value < 0) buf += '-';
    std::reverse( buf.begin(), buf.end() );
    return ZString(buf);
}

int ZString::tint(){
    const char *str = data.c_str();
    return atoi(str);
}

char &ZString::operator[](zu64 index){
    if(index > size())
        return byte;
    else
        return data[index];
}

int ZString::size(){
    return data.size();
}
int ZString::length(){
    return data.length();
}

int ZString::count(std::string needle){
    std::string haystack = data;
    int count = 0;
    for(unsigned i = 0; i < haystack.length(); ++i){
        if(haystack[i] == needle[0]){
            bool good = true;
            for(unsigned g = i, j = 0; j < needle.length(); ++g, ++j){
                if(haystack[g] != needle[j]){
                    good = false;
                    break;
                }
            }
            if(good)
                ++count;
        }
    }
    return count;
}

char ZString::first(){
    if(size() >= 1)
        return data[0];
    else
        return '\0';
}
char ZString::last(){
    if(size() >= 1)
        return data[size()-1];
    else
        return '\0';
}

void ZString::clear(){
    data.clear();
}
bool ZString::isEmpty(){
    return data.empty();
}

bool ZString::startsWith(ZString test, bool ignorews){
    std::string start = test.str();
    bool started;
    if(ignorews)
        started = false;
    else
        started = true;
    unsigned j = 0;
    for(unsigned i = 0; i < data.size(); ++i){
        if(data[i] == start[j]){
            if(j == test.size()-1)
                return true;
            started = true;
            ++j;
        } else if(data[i] == ' ' || data[i] == '\t'){
            if(started)
                return false;
        } else {
            return false;
        }
    }
    return true;
}
bool ZString::beginsWith(ZString str){
    return startsWith(str, false);
}

bool ZString::endsWith(ZString test){
    if(test.size() > size())
        return false;
    std::string end = data.substr(data.size()-test.size(), test.size());
    return test.str() == end;
}

ZString &ZString::substr(zu64 pos){
    data = substr(data, pos).str();
    return *this;
}
ZString ZString::substr(ZString str, zu64 pos){
    return ZString(str.str().substr(pos, std::string::npos));
}

ZString &ZString::substr(zu64 pos, zu64 len){
    data = substr(data, pos, len).str();
    return *this;
}
ZString ZString::substr(ZString str, zu64 pos, zu64 len){
    return ZString(str.str().substr(pos, len));
}

unsigned long ZString::findFirst(ZString str, ZString find){
    return str.str().find(find.str());
}

ZString &ZString::replace(zu64 pos, zu64 len, ZString after){
    data = replace(data, pos, len, after).str();
    return *this;
}
ZString ZString::replace(ZString str, zu64 pos, zu64 len, ZString after){
    return substr(str, 0, pos) + after + substr(str, pos+len);
}

ZString &ZString::replaceRecursive(ZString before, ZString after, unsigned max){
    data = replace(data, before, after, max).str();
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
    data = replace(data, before, after, max).str();
    return *this;
}
ZString ZString::replace(ZString str, ZString before, ZString after, unsigned max){
    if(str.isEmpty())
        return ZString();
    if(before.isEmpty())
        return str;
    bool unlim = false;
    if(max == (unsigned)-1)
        unlim = true;

    bool found = true;
    unsigned count = 0;
    unsigned last = 0;
    while(found && (count < max || unlim)){
        //unsigned long loc = findFirst(str, before);
        unsigned long loc = str.str().find(before.str(), last);
        if(loc != (unsigned long)-1){
            str.str().replace(loc, before.size(), after.str());
            last = loc + after.size();
            ++count;
        } else {
            found = false;
        }
    }
    return str;
}

/*
        if(str.size() >= before.size()){
            for(unsigned long i = 0; i < str.size(); ++i){
                bool match = true;
                unsigned long last = i;
                for(unsigned long j = 0; j < before.size(); ++j){
                    if(str[i+j] != before[j]){
                        match = false;
                        break;
                    }
                    last = i+j;
                }
            }
        }

        for(unsigned long i = 0; i < str.size(); ++i){
            if(str[i] == before.first()){
                bool match = true;
                int last = 0;
                for(unsigned long j = 0; j < before.size(); ++j){
                    if(str[i+j] != before[j]){
                        match = false;
                        break;
                    }
                    last = i + j;
                }
                if(match){
                    tmp << str.substr(0, i, false);
                    if(!after.isEmpty()){
                        tmp << after;
                    }
                    str = str.substr(last + 1, false);
                    //i = -1;
                }
            }
        }
*/

/*ZString ZString::replace(ZString zbefore, ZString zafter, bool modify){
    if(isEmpty())
        return ZString();
    if(zbefore.isEmpty())
        return *this;
    //std::string before = zbefore.str();
    //std::string after = zafter.str();
    //std::string tmpdata = data;
    ZString tmpdata = data;
    //std::string tmp;
    ZString tmp;
    for(long i = 0; i < tmpdata.size(); ++i){
        if(tmpdata[i] == zbefore.first()){
            bool match = true;
            int last = 0;
            for(unsigned j = 0; j < zbefore.size(); ++j){
                if(tmpdata[i+j] != zbefore[j]){
                    match = false;
                    break;
                }
                last = i + j;
            }
            if(match){
                //std::string pre = tmpdata.substr(0, i);
                //std::string suff = tmpdata.substr(last+1);
                //tmp = tmp.append(pre);
                tmp << tmpdata.substr(0, i, false);
                if(!zafter.isEmpty()){
                    //tmp = tmp.append(zafter.str());
                    tmp << zafter;
                }
                //tmpdata = suff;
                tmpdata = tmpdata.substr(last + 1, false);
                i = -1;
            }
        }
    }
    //tmp = tmp.append(tmpdata);
    tmp << tmpdata;

    if(modify){
        data = tmp.str();
        return *this;
    } else {
        return tmp;
    }
}*/

ZString ZString::findFirstBetween(ZString opening_string, ZString closing_string){
    std::string pre = opening_string.str();
    std::string post = closing_string.str();
    if(pre.size() <= 0 || post.size() <= 0)
        return ZString();
    std::string tmp = data;
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
    ZString tmp = data;
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
    data = tmp.str();
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
    return ZString(data);
}
ZString ZString::label(ZString labeltxt, ZString value, bool modify){
    ZString label = ZString("<?").append(labeltxt).append("?>");
    if(modify)
        return replace(label, value);
    else
        return replace(data, label, value);
}

ArZ ZString::explode(char delim){
    ArZ out;
    std::string str = data;
    for(unsigned i = 0; i < str.length(); ++i){
        if(str[i] == '"'){
            for(unsigned j = i; j < str.length(); ++j){
                if(str[j] == '"'){
                    out.push(str.substr(0, j));
                    str = str.substr(j+1, str.length());
                    i = -1;
                }
            }
        } else if(str[i] == delim){
            out.push(str.substr(0, i));
            str = str.substr(i+1, str.length());
            i = -1;
        }
    }
    out.push(str);
    return out;
}
ArZ ZString::strict_explode(char delim){
    ArZ out;
    std::string str = data;
    for(unsigned i = 0; i < str.length(); ++i){
        if(str[i] == delim && str[i-1] != '\\'){
            std::string substr = str.substr(0, i);
            if(substr != ""){
                out.push(substr);
            }
            str = str.substr(i+1, str.length());
            i = -1;
        }
    }
    out.push(str);
    return out;
}

ZString ZString::strip(char target, bool modify){
    ZString tmp = data;
    for(unsigned i = 0; i < data.length(); ++i){
        if(data[i] == target){
            tmp = data.substr(i+1, data.length());
        } else {
            break;
        }
    }
    data = tmp.str();
    for(unsigned i = 0; i < data.length(); ++i){
        int curr = data.length() - 1 - i;
        if(data[curr] == target){
            tmp = data.substr(0, curr);
        } else {
            break;
        }
    }

    if(modify){
        data = tmp.str();
        return ZString(data);
    } else {
        return tmp;
    }
}

ZString ZString::removeWhitespace(){
    replace("  ", "");
    replace("\n", "");
    replace("\r", "");
    return *this;
}

ZString ZString::invert(bool modify){
    std::string buff;
    for(unsigned i = data.length(); i > 0; --i){
        buff += data[i];
    }

    if(modify){
        data = buff;
        return ZString(data);
    } else {
        return ZString(buff);
    }
}

ZString ZString::toLower(bool modify){
    std::string tmp = data;
    for(unsigned i = 0; i < data.size(); ++i){
        // Custom tolower()
        //if((int)tmp[i] >= 65 && (int)tmp[i] <= 90)
        //    tmp[i] = (char)((int)tmp[i] + 32);
        tmp[i] = tolower(tmp[i]);
    }
    if(modify){
        data = tmp;
        return ZString(data);
    } else {
        return ZString(tmp);
    }
}

ZString ZString::duplicate(unsigned iter, bool modify){
    ZString tmp;
    for(unsigned i = 0; i < iter; ++i){
        tmp << data;
    }
    if(modify){
        data = tmp.str();
        return ZString(data);
    } else {
        return tmp;
    }
}

ZString ZString::popLast(){
    return substr(0, size()-1);
}

std::ostream &operator<<(std::ostream& lhs, ZString rhs){
    lhs << rhs.str();
    return lhs;
}

}
