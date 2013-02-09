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
ZString &ZString::operator+=(ZString str){
    data.concat(str.ZAc());
    return *this;
}
ZString &ZString::operator<<(ZString str){
    return operator+=(str);
}

ZString::ZString(std::string str){
    operator=(str);
}
std::string &ZString::str(){
    return data;
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
    operator=(str);
}
char* ZString::c(){
    char str[size()];
    return strcpy(str, data.c_str());
}

ZString::ZString(const char *str){
    operator=(str);
}
const char* ZString::cc(){
    return data.c_str();
}

ZString::ZString(char str){
    operator=(str);
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
    std::stringstream ss; std::string out;
    ss << num; ss >> out;
    data = out;
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

ZString ZString::replace(std::string before, std::string after, bool modify){
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

ZString ZString::label(std::string labeltxt, ZString value, bool modify){
    std::string label = std::string("<?").append(labeltxt).append("?>");
    return replace(label, value.str(), modify);
}

AsArZ ZString::explode(char delim){
    AsArZ out;
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
AsArZ ZString::strict_explode(char delim){
    AsArZ out;
    std::string str = data;
    for(unsigned i = 0; i < str.length(); ++i){
        if(str[i] == delim && str[i-1] != '\\'){
            string substr = str.substr(0, i);
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

ZString ZString::substr(int pos, bool modify){
    std::string tmp = data;
    tmp.substr(pos);

    if(modify){
        data = tmp;
        return ZString(data);
    } else {
        return ZString(tmp);
    }
}

ZString ZString::substr(int pos, int npos, bool modify){
    std::string tmp = data;
    tmp.substr(pos, npos);

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

ostream &operator<<(ostream& lhs, ZString rhs){
    lhs << rhs.str();
    return lhs;
}
