#include "zpath.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "zfile.h"

#ifdef COMPILER_MINGW
    #include <direct.h>
#endif

namespace LibChaos {

ZPath::ZPath() : _absolute(false){
#if PLATFORM == WINDOWS
    // Detect prefix (drive letter) on Windows
    size_t sz = 1024;
    char *ptr = new char[sz];
    getcwd(ptr, sz);
    ZString pth = ptr;
    _prefix = ZString::getUntil(pth, ZPATH_DEFAULT_DELIM);
#endif
    // Other platforms have default empty prefix
}

ZPath::ZPath(ZString path) : ZPath(){
    fromStr(path);
}
//ZPath::ZPath(std::string path) : ZPath(){
//    fromStr(path);
//}
ZPath::ZPath(const char *path) : ZPath(){
    fromStr(path);
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
            _data.pop(i);
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

ZPath &ZPath::concat(ZPath path){
    _data.concat(path.data());
    return *this;
}
ZPath ZPath::operator+(ZPath path){
    ZPath tmp(*this);
    return tmp.concat(path);
}
ZPath &ZPath::operator<<(ZPath path){
    return concat(path);
}

ZString &ZPath::operator[](unsigned inx){
    return _data[inx];
}

ZString &ZPath::last(){
    return _data.back();
}

ZPath ZPath::pwd(){
    size_t sz = 1024;
    char *ptr = new char[sz];
    getcwd(ptr, sz);
    ZPath path(ptr);
    delete[] ptr;
    return path;
}

ZPath &ZPath::relativeTo(ZPath path){
    // Make both paths absolute
    if(!absolute())
        getAbsolute();
    if(!path.absolute())
        path.getAbsolute();

    ZPath up;
    zu64 match_len = 0;
    for(zu64 i = 0; i < path.size() && i < _data.size(); ++i){
        if(path[i] != _data[i]){
            break;
        }
        ++match_len;
        up.concat("..");
    }
    _data.popFrontCount(match_len);
    path.data().popFrontCount(match_len);

    up.concat(*this);
    *this = up;
    sanitize();
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

bool ZPath::childTo(ZPath path){
    if(path.depth() > depth())
        return false;
    for(unsigned i = 0; i < path.depth(); ++i){
        if(path[i] != _data[i])
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
                    tmp2.pop(tmp2.size()-1);
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
        cwd.concat(*this);
        cwd.sanitize();
        *this = cwd;
    }
    return *this;
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

bool ZPath::makeDir(ZPath dir){
    struct stat st_buf;
    int ret = stat(dir.str().cc(), &st_buf);
    if(ret == 0){
        if(S_ISDIR(st_buf.st_mode)){
            return true;
        } else {
            return false;
        }
    }
#if COMPILER == MINGW
    const char *tmp = dir.str().cc();
    return (mkdir(tmp) == 0);
#elif COMPILER == GCC
    return (mkdir(dir.str().cc(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0);
#endif
}

bool ZPath::createDirsTo(){
    ZPath orig = *this;
    ZPath cur = orig;
    for(zu64 i = 0; i < orig.size()-1; ++i){
        cur.concat(orig[i]);
        if(!makeDir(cur))
            return false;
    }
    return true;
}

ZString ZPath::str(ZString delim) const {
    ZString tmp;
    if(_absolute){
        tmp = _prefix + delim;
    }
    ArZ data = _data;
//#if PLATFORM != WINDOWS
//    // Escape spaces
//    for(zu64 i = 0; i < data.size(); ++i){
//        data[i].replace(" ", "\\ ");
//    }
//#endif
    ZString str = ZString::compound(data, delim);
    tmp += str;
    return tmp;
}

}
