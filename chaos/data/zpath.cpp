#include "zpath.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "zfile.h"

#ifdef COMPILER_MINGW
    #include <direct.h>
#endif

namespace LibChaos {

ZPath::ZPath() : _data(), absolute(false){}

ZPath::ZPath(const char *path){
    fromStr(path);
}
ZPath::ZPath(std::string path){
    fromStr(path);
}
ZPath::ZPath(ZString path){
    fromStr(path);
}
ZPath::ZPath(ArZ arr){
    ZString buff;
    for(zu64 i = 0; i < arr.size(); ++i)
        buff << "/" << arr[i];
    fromStr(buff);
}

bool ZPath::operator==(ZPath pth){
#if PLATFORM == WINDOWS
    if((_data == pth.dat()) && (absolute == pth.abs()) && (drive == pth.drv()))
        return true;
    return false;
#else
    return (_data == pth.dat() && absolute == pth.abs());
#endif
}

void ZPath::fromStr(ZString path){
#if PLATFORM == WINDOWS
    ArZ tmp;
    tmp = path.explodeList(2, ZPATH_DELIM, '/');
    zu64 i = 0;
    while(i < tmp.size()){
        if(tmp[i].isEmpty()){
            tmp.erase(i);
            i = 0;
        }
        ++i;
    }
    if(tmp[0].size() == 2 && tmp[0][1] == ':'){
        drive = tmp[0][0];
        tmp.popFront();
        _data = tmp;
        absolute = true;
    } else {
        _data = tmp;
        drive = '!';
        absolute = false;
    }
#else
    _data = path.explode(ZPATH_DELIM);
    zu64 i = 0;
    while(i < _data.size()){
        if(_data[i].isEmpty()){
            _data.erase(i);
            i = 0;
        }
        ++i;
    }
    absolute = (path[0] == ZPATH_DELIM);
#endif
    for(zu64 i = 0; i < _data.size(); ++i){
        if(_data[i] == "."){
            _data.pop(i);
        }
    }
}

ZPath &ZPath::concat(ZPath path){
    _data.concat(path.dat());
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
    delete ptr;
    return path;
}

ZPath &ZPath::relTo(ZPath path){
    if(!abs())
        getAbs();
    if(!path.abs())
        path.getAbs();
    unsigned match_len = 0;
    for(unsigned i = 0; i < path.size() && i < _data.size(); ++i){
        if(path[i] != _data[i]){
            break;
        }
        match_len = i + 1;
    }
    _data.popFrontCount(match_len);
    path.dat().popFrontCount(match_len);
    ZString up = "../";
    up.duplicate(path.depth());
    ArZ tmp = _data;
    fromStr(up);
    _data.concat(tmp);
    return *this;
}

ZPath &ZPath::parent(){
    if(size() > 1)
        _data.popBack();
    else if(!abs())
        _data.push("..");
    return *this;
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
                absolute = false;
            } else {
                if(prevup){
                    tmp2.push("..");
                } else {
                    tmp2.pop(tmp2.size()-1);
                }
            }
            prevup = true;
        } else {
            tmp2.push(pth);
            prevup = false;
        }
    }
    _data = tmp2;
    return *this;
}

ZPath ZPath::getAbs(ZPath path){
    if(!path.abs()){
        ZPath cwd = pwd();
        cwd.concat(path);
        cwd.sanitize();
        return cwd;
    }
    return path;
}
ZPath &ZPath::getAbs(){
    *this = getAbs(*this);
    return *this;
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
    ZPath orig(_data);
    orig.abs() = absolute;
#if PLATFORM == WINDOWS
    orig.drv() = drive;
#endif
    ZPath cur;
    cur.abs() = orig.abs();
#if PLATFORM == WINDOWS
    cur.drv() = orig.drv();
#endif
    for(unsigned i = 0; i < orig.size()-1; ++i){
        cur.concat(orig[i]);
        if(!makeDir(cur))
            return false;
    }
    return true;
}

ZString ZPath::str(char delim){
    ZString tmp;
    if(absolute){
#if PLATFORM == WINDOWS
        tmp << drive << ':';
#endif
        tmp << delim;
    }
    for(unsigned i = 0; i < _data.size(); ++i){
        tmp << _data[i] << delim;
    }
    if(_data.size() > 0)
        tmp.popLast();
    return tmp;
}

unsigned ZPath::depth(){
    return size();
}
unsigned ZPath::size(){
    return _data.size();
}

ArZ &ZPath::dat(){
    return _data;
}
bool &ZPath::abs(){
    return absolute;
}

#if PLATFORM == WINDOWS
char &ZPath::drv(){
    return drive;
}
#endif

}
