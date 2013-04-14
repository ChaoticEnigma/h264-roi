#include "zstring.h"

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
    ZArray<char> tmp = data;
    tmp.concat(str.ZAc());
    return ZString(tmp);
}
ZString &ZString::append(ZString str){
    data.concat(str.ZAc());
    return *this;
}

ZString::ZString(int num) : data(){
    char *tmp;
    sprintf(tmp, "%d", num);
    data = ZArray<char>(tmp, strlen(tmp));
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

ZString ZString::substr(int pos, int npos, bool modify){
    ZArray<char> tmp = data.subarr(pos, npos);

    if(modify){
        data = tmp;
        return ZString(data);
    } else {
        return ZString(tmp);
    }
}

/*ZArray<ZString::SubZStr> ZString::findAll(ZString target){
    ZArray<SubZStr> out;
    bool candidate = false;
    unsigned long startbuf = 0;
    for(unsigned long i = 0; i < size(); ++i){
        if(candidate){
            if(data[i] == target[i - startbuf]){
                if(target[i - startbuf] == target.size() - 1){
                    SubZStr sub;
                    sub.start = startbuf;
                    sub.end = i;
                    out.push_back(sub);
                    candidate = false;
                }
            } else {
                candidate = false;
            }
        } else {
            if(data[i] == target[0]){
                candidate = true;
                startbuf = i;
            }
        }
    }
    return out;
}*/

ZArray<ZString::SubZStr> ZString::findAll(ZString target){
    ZArray<SubZStr> out;
    unsigned long ti = 0;
    unsigned long startbuf = 0;
    for(unsigned long i = 0; i < size(); ++i){
        if(data[i] == target[ti]){
            ++ti;
            if(startbuf == 0)
                startbuf = i;
            if(ti == target.size()-1){
                SubZStr sub;
                sub.start = startbuf;
                sub.end = i+1;
                out.push_back(sub);
                ti = 0;
                startbuf = 0;
            }
        } else {
            ti = 0;
            startbuf = 0;
        }
    }
    return out;
}

// Replace v1
/*ZString ZString::replace(ZString before, ZString after, bool modify){
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
}*/

// Replace v2
ZString ZString::replace(ZString before, ZString after, bool modify){
    ZString tmpdata = data;
    ZString tmp;
    ZArray<SubZStr> locs = findAll(before);
    for(unsigned long i = locs.size()-1; i >= 0; --i){

    }
    return tmp;
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
    for(long i = 0; i < rhs.size(); ++i)
        lhs << rhs[i];
    return lhs;
}
