#include "zstring.h"

ZString::ZString(){
    data = ZArray<char>();
}

ZString::ZString(ZArray<char> str){
    data = str;
}

ZString &ZString::operator=(ZString str){
    data.clear();
    for(unsigned i = 0; i < str.size(); ++i)
        data.push_back(str[i]);
    return *this;
}
bool ZString::operator==(ZString str){
    if(str.ZAc() == data)
        return true;
    return false;
}
bool ZString::operator!=(ZString str){
    if(str.ZAc() == data)
        return false;
    return true;
}
ZString ZString::operator+(ZString str){
    return ZString(data.concat(str.ZAc()));
}
ZString &ZString::append(ZString str){
    data.concat(str.ZAc());
    return *this;
}
ZString &ZString::operator+=(ZString str){
    return append(str);
}
ZString &ZString::operator<<(ZString str){
    return append(str);
}

ZString::ZString(std::string str){
    data = ZArray<char>(str.c_str());
}
std::string ZString::str(){
    std::string tmp;
    tmp.clear();
    for(long i = 0; i < data.size(); ++i)
        tmp[i] = data[i];
    return tmp;
}

#ifdef ZSTRING_USE_QT
ZString::ZString(QString str){
    operator=(str);
}
QString ZString::QS(){
    return QString::fromStdString(data);
}

ZString::ZString(QByteArray str){
    operator=(str);
}
QByteArray ZString::QBA(){
    return QByteArray(data.c_str());
}
#endif

ZString::ZString(char *str){
    data = ZArray<char>(str);
}
char *ZString::c(){
    return data.c_style();
}

ZString::ZString(const char *str){
    std::cout << str << std::endl;
    data = ZArray<char>(str);
    return;
}
const char* ZString::cc(){
    return (const char *)c();
}

ZString::ZString(char ch){
    data = ZArray<char>();
    data.push_back(ch);
}

#ifdef ZSTRING_USE_QT
ZString::ZString(qint64 num){
    const char *str = reinterpret_cast<const char *>(&num);
    //std::stringstream ss; std::string out;
    //ss << num; ss >> out;
    data = str;
}
#endif

ZString::ZString(int num){
    char *tmp;
    itoa(num, tmp, 10);
    data = ZArray<char>();
}
int ZString::tint(){
    return atoi(data.c_style());
}

int ZString::count(ZString needle){
    ZString haystack = data.c_style();
    int count = 0;
    for(unsigned i = 0; i < haystack.length(); ++i){
        if(haystack[i] == needle[0]){
            bool good = true;
            for(unsigned g = i, j = 0; j < needle.size(); ++g, ++j){
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

ZString ZString::replace(ZString before, ZString after, bool modify){
    ZString tmpdata = data;
    ZString tmp = "";
    for(unsigned i = 0; i < tmpdata.size(); ++i){
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
                ZString pre = tmpdata.substr(0, i);
                ZString suff = tmpdata.substr(last+1, tmpdata.size());
                tmp = tmp.append(pre).append(after);
                tmpdata = suff;
                i = -1;
            }
        }
    }
    tmp = tmp.append(tmpdata);

    if(modify){
        data = tmp.ZAc();
        return ZString(data);
    } else {
        return ZString(tmp);
    }
}

ZString ZString::label(std::string labeltxt, ZString value, bool modify){
    std::string label = std::string("<?").append(labeltxt).append("?>");
    return replace(label, value.str(), modify);
}

ZString ZString::strip(char target, bool modify){
    ZArray<char> tmp = data;
    for(unsigned i = 0; i < data.size(); ++i){
        if(data[i] == target){
            tmp = data.subarr(i+1, data.size());
        } else {
            break;
        }
    }
    data = tmp;
    for(unsigned i = 0; i < data.size(); ++i){
        int curr = data.size() - 1 - i;
        if(data[curr] == target){
            tmp = data.subarr(0, curr);
        } else {
            break;
        }
    }

    if(modify){
        data = tmp;
        return ZString(data);
    } else {
        return ZString(tmp);
    }
}

ZString ZString::substr(int pos, int npos, bool modify){
    ZArray<char> tmp = data.subarr(pos, npos);

    if(modify){
        data = tmp;
        return ZString(data);
    } else {
        return ZString(tmp);
    }
}

ZString ZString::invert(bool modify){
    ZArray<char> buff;
    for(unsigned i = data.size(); i > 0; --i){
        buff.push_back(data[i]);
    }

    if(modify){
        data = buff;
        return ZString(data);
    } else {
        return ZString(buff);
    }
}

ZString ZString::toLower(bool modify){
    ZArray<char> tmp = data;
    for(unsigned i = 0; i < data.size(); ++i){
        tmp[i] = tolower(tmp[i]);
    }
    if(modify){
        data = tmp;
        return ZString(data);
    } else {
        return ZString(tmp);
    }
}

std::ostream &operator<<(std::ostream& lhs, ZString rhs){
    lhs << rhs.c();
    return lhs;
}
