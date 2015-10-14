/*******************************************************************************
**                                  LibChaos                                  **
**                                  zpath.cpp                                 **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zpath.h"

#include "zfile.h"

#if COMPILER == MSVC
    #include <direct.h>
#elif COMPILER == MINGW
    #include <direct.h>
    #include <unistd.h>
#else
    #include <unistd.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

namespace LibChaos {

ZPath::ZPath() : _absolute(false){
#if PLATFORM == WINDOWS
    // Detect prefix (drive letter) on Windows
    size_t sz = 1024;
    char *ptr = new char[sz];
#if COMPILER == MSVC
    _getcwd(ptr, (int)sz);
#else
    getcwd(ptr, sz);
#endif
    ZString pth = ptr;
    _prefix = ZString::getUntil(pth, ZPATH_DEFAULT_DELIM);
#endif
    // Other platforms have default empty prefix
}

ZPath::ZPath(ZString str) : ZPath(){
    fromStr(str);
}
//ZPath::ZPath(std::string path) : ZPath(){
//    fromStr(path);
//}
ZPath::ZPath(const char *str) : ZPath(){
    fromStr(str);
}

ZPath::ZPath(ArZ arr) : ZPath(){
    ZString buff;
    for(zu64 i = 0; i < arr.size(); ++i)
        buff << "/" << arr[i];
    fromStr(buff);
}

bool ZPath::operator==(ZPath pth){
    return (_data == pth.data() && _absolute == pth.absolute());
}

void ZPath::fromStr(ZString path){
#if PLATFORM == WINDOWS
    if(path.size() >= 2 && path[1] == ':'){
        _absolute = true;
        _prefix = ZString::substr(path, 0, 2);
        path.substr(_prefix.size());
    } else
#endif
    if(path.size() >= 1 && isDelim(path[0])){
        _absolute = true;
    }

#if PLATFORM == WINDOWS
    _data = path.explodeList(2, ZPATH_DEFAULT_DELIM, '/');
#else
    _data = path.explode(ZPATH_DEFAULT_DELIM);
#endif

    for(zu64 i = 0; i < _data.size(); ++i){
        if(_data[i] == "."){
            _data.erase(i);
            --i;
            continue;
        }
//#if PLATFORM != WINDOWS
//        _data[i].replace("\\ ", " ");
//#endif
    }
    //sanitize();
}

bool ZPath::isDelim(char ch){
#if PLATFORM == WINDOWS
    if(ch == '\\')
        return true;
#endif
    return ch == '/';
}

ZPath &ZPath::append(ZPath path){
    _data.append(path.data());
    return *this;
}

ZPath ZPath::concat(ZPath path) const {
    ZPath tmp(*this);
    tmp.append(path.data());
    return tmp;
}

ZPath ZPath::pwd(){
    size_t sz = 1024;
    char *ptr = new char[sz];
#if COMPILER == MSVC
    _getcwd(ptr, (int)sz);
#else
    getcwd(ptr, sz);
#endif
    ZPath path(ptr);
    delete[] ptr;
    return path;
}

ZPath &ZPath::relativeTo(ZPath base){
    // Get path to here:
    // /a/path/here/test.txt
    // from here:
    // /this/path/to/another/place
    // =
    // ../../../../../a/path/here/test.txt

    // Get path to here:
    // /some/short/thing.obj
    // from here:
    // /some/short/other/path
    // =
    // ../../thing.obj

    ZPath path = *this;

    // Make both paths absolute
    path.getAbsolute();
    base.getAbsolute();

    // Get number of matching parts from beginning of path
    // Build path of ".."s
    zu64 match_len = 0;
    for(zu64 i = 0; i < path.size() && i < base.size(); ++i){
        if(base[i] != path[i])
            break;
        ++match_len;
    }
    // Remove common path parts
    path.data().popFrontCount(match_len);
    base.data().popFrontCount(match_len);

    // Build path of ".."s for remaining parts in relative
    ZPath up;
    for(zu64 i = 0; i < base.size(); ++i){
        up.append("..");
    }
    // Combine
    up.append(path);
    up.sanitize();

    *this = up;
    return *this;
}

ZPath ZPath::relativeTo(ZPath path, ZPath abs){
    return path.relativeTo(abs);
}

ZPath &ZPath::parent(){
    if(size() > 1)
        _data.popBack();
    else if(!absolute())
        _data.push("..");
    return *this;
}
ZPath ZPath::parent(ZPath path){
    return path.parent();
}

// NOTE: Needs REWRITE
bool ZPath::childTo(ZPath path) const {
    // Get absolute and clean paths
    ZPath base = *this;
    base.getAbsolute();
    path.getAbsolute();

    if(base.depth() >= path.depth())
        return false;
    for(zu64 i = 0; i < base.size(); ++i){
        if(base[i] != path[i])
            return false;
    }
    return true;
}

ZPath &ZPath::sanitize(){
    // /some/odd/../complic/ated/../path
    // /some/complic/path

    // ../../another/path
    // ../../another/path

    // /some/../../../path
    // ../../path

    ArZ tmp = _data;
    ArZ tmp2;
    bool prevup = false;
    for(unsigned i = 0; i < tmp.size(); ++i){
        ZString pth = tmp[i];
        if(pth == ".."){
            if(i <= 0){
                tmp2.push("..");
                _absolute = false;
            } else {
                if(prevup){
                    tmp2.push("..");
                } else {
                    tmp2.remove(tmp2.size() - 1);
                }
            }
            prevup = true;
        } else if(pth == "."){
            // Discard
            prevup = false;
        } else {
            tmp2.push(pth);
            prevup = false;
        }
    }
    _data = tmp2;
    return *this;
}

ZPath ZPath::sanitize(ZPath path){
    return path.sanitize();
}

ZPath &ZPath::getAbsolute(){
    if(!absolute()){
        ZPath cwd = pwd();
        cwd.append(*this);
        cwd.sanitize();
        *this = cwd;
    }
    return *this;
}

ZString ZPath::getExtension() const {
    if(size() == 0)
        return ZString();
    ArZ parts = last().explode('.');
    if(parts.size() == 0)
        return ZString();
    return "." + parts.back();
}

ZPath ZPath::getAbsolute(ZPath path){
    return path.getAbsolute();
}

bool ZPath::valid(){
#if PLATFORM == WINDOWS
    for(unsigned i = 0; i < _data.size(); ++i){
        if(_data[i].count("/") > 0 ||
           _data[i].count("\\") > 0 ||
           _data[i].count(":") > 0 ||
           _data[i].count("*") > 0 ||
           _data[i].count("\"") > 0 ||
           _data[i].count("?") > 0 ||
           _data[i].count("<") > 0 ||
           _data[i].count(">") > 0 ||
           _data[i].count("|") > 0)
            return false;
    }
    return true;
#else
    for(unsigned i = 0; i < _data.size(); ++i){
        if(_data[i].count("/") > 0)
            return false;
    }
    return true;
#endif
}

ZPath &ZPath::fix(){
    // Much hack, wow
    if(!valid()){
        fromStr(str());
    }
    return *this;
}

ZString ZPath::str(ZString delim) const {
    ZString tmp;
    if(_absolute){
        tmp = _prefix + delim;
    }
//    ArZ data = _data;
//#if PLATFORM != WINDOWS
//    // Escape spaces
//    for(zu64 i = 0; i < data.size(); ++i){
//        data[i].replace(" ", "\\ ");
//    }
//#endif
    return tmp + ZString::compound(_data, delim);
}

}
