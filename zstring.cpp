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
    if(str){
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
    if(str){
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

std::string ZString::ItoS(long int value, int base) {
    std::string buf;
    if (base < 2 || base > 16) return buf;
    enum { kMaxDigits = 35 };
    buf.reserve( kMaxDigits );
    long int quotient = value;
    do {
        buf += "0123456789abcdef"[ std::labs( quotient % base ) ];
        quotient /= base;
    } while ( quotient );
    if ( value < 0) buf += '-';
    std::reverse( buf.begin(), buf.end() );
    return buf;
}

ZString::ZString(int num){
    data = ItoS(num, 10);
}
int ZString::tint(){
    const char *str = data.c_str();
    return atoi(str);
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

ZString ZString::replace(ZString zbefore, ZString zafter, bool modify){
    std::string before = zbefore.str();
    std::string after = zafter.str();
    std::string tmpdata = data;
    std::string tmp = "";
    for(unsigned i = 0; i < tmpdata.length(); ++i){
        if(tmpdata[i] == before[0]){
            bool match = true;
            int last = 0;
            for(unsigned j = 0; j < before.length(); ++j){
                if(tmpdata[i+j] != before[j]){
                    match = false;
                    break;
                }
                last = i + j;
            }
            if(match){
                std::string pre = tmpdata.substr(0, i);
                std::string suff = tmpdata.substr(last+1);
                tmp = tmp.append(pre).append(after);
                tmpdata = suff;
                i = -1;
            }
        }
    }
    tmp = tmp.append(tmpdata);

    if(modify){
        data = tmp;
        return ZString(data);
    } else {
        return ZString(tmp);
    }
}

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
    return replace(label, value, modify);
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

ZString ZString::substr(int pos, bool modify){
    std::string tmp = data;
    tmp = tmp.substr(pos);

    if(modify){
        data = tmp;
        return ZString(data);
    } else {
        return ZString(tmp);
    }
}

ZString ZString::substr(int pos, int npos, bool modify){
    std::string tmp = data;
    tmp = tmp.substr(pos, npos);

    if(modify){
        data = tmp;
        return ZString(data);
    } else {
        return ZString(tmp);
    }
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

ZString ZString::popLast(bool modify){
    return substr(0, size()-1, modify);
}

std::ostream &operator<<(std::ostream& lhs, ZString rhs){
    lhs << rhs.str();
    return lhs;
}

}
