#include "zpath.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "zfile.h"

namespace LibChaos {

ZPath::ZPath() : data(), absolute(false){}

ZPath::ZPath(const char *path){
    fromZString(path);
}
ZPath::ZPath(std::string path){
    fromZString(path);
}
ZPath::ZPath(ZString path){
    fromZString(path);
}
/*ZPath::ZPath(ZString path, bool absl){
    fromZString(path);
    absolute = absl;
}
ZPath::ZPath(ArZ arr){
    data = arr;
    absolute = false;
}*/

void ZPath::fromZString(ZString path){
#ifdef PLATFORM_WINDOWS
    ArZ tmp;
    tmp = path.explodeList(2, ZPATH_DELIM, '/');
    //tmp.clean();
    for(zu64 i = 0; i < tmp.size(); ++i){
        if(tmp[i].isEmpty())
            tmp.erase(i);
    }
    if(tmp[0].size() == 2 && tmp[0][1] == ':'){
        drive = tmp[0][0];
        tmp.popFront();
        data = tmp;
        absolute = true;
    } else {
        data = tmp;
        drive = '!';
        absolute = false;
    }
#else
    data = path.explode(ZPATH_DELIM);
    //data.clean();
    for(zu64 i = 0; i < data.size(); ++i){
        if(data[i].isEmpty())
            data.erase(i);
    }
    absolute = (path[0] == ZPATH_DELIM);
#endif
}

/*ZPath &ZPath::operator=(ZPath path){
    data = path.dat();
    absolute = path.abs();
#ifdef PLATFORM_WINDOWS
    drive = path.drv();
#endif
    return *this;
}*/

/*ZPath &ZPath::operator=(ZString str){
    if(str[0] == '/')
        absolute = true;
    else
        absolute = false;
    data = str.explode('/');
    data.clean();
    return *this;
}*/

/*ZPath &ZPath::operator=(ArZ arr){
    absolute = false;
    data = arr;
    return *this;
}*/

/*bool ZPath::operator==(ZPath path){
    if(path.depth() != data.size())
        return false;
    for(unsigned i = 0; i < path.size(); ++i){
        if(path[i] != data[i])
            return false;
    }
    return true;
}*/

ZPath &ZPath::concat(ZPath path){
    data.concat(path.dat());
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
    return data[inx];
}

ZString &ZPath::last(){
    return data.back();
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
    int match_len = 0;
    for(unsigned i = 0; i < path.size() && i < data.size(); ++i){
        if(path[i] != data[i]){
            break;
        }
        match_len = i + 1;
    }
    data.popFrontCount(match_len);
    path.dat().popFrontCount(match_len);
    ZString up = "../";
    up.duplicate(path.depth());
    ZPath tmp = up;
    tmp.dat().concat(data);
    operator=(tmp);
    return *this;
}

ZPath &ZPath::parent(){
    if(size() < 1)
        data.popBack();
    else
        data.push("..");
    return *this;
}

bool ZPath::childTo(ZPath path){
    if(path.depth() > depth())
        return false;
    for(unsigned i = 0; i < path.depth(); ++i){
        if(path[i] != data[i])
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

    ArZ tmp = data;
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
    data = tmp2;
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
    for(unsigned i = 0; i < data.size(); ++i){
        if(data[i].count("/") > 0 ||
           data[i].count("\\") > 0 ||
           data[i].count(":") > 0 ||
           data[i].count("*") > 0 ||
           data[i].count("?") > 0 ||
           data[i].count("<") > 0 ||
           data[i].count(">") > 0 ||
           data[i].count("|") > 0)
            return false;
    }
    return true;
}

ZPath &ZPath::fix(){
    if(!valid()){
        operator=(str());
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
            ZFile::remove(dir);
        }
    }
#ifdef COMPILER_MINGW
    return (mkdir(dir.str().cc()) == 0);
#else
    return (mkdir(dir.str().cc(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0);
#endif
}

bool ZPath::createDirsTo(){
    ZPath orig = *this;
    orig.getAbs();
    for(unsigned i = 0; i < orig.size()-1; ++i){
        ZPath tmp;
        for(unsigned j = 0; j < i+1; ++j){
            tmp.dat().push(orig[j]);
        }
        tmp.abs() = true;
        if(!makeDir(tmp))
            return false;
    }
    return true;
}

ZString ZPath::str(){
#ifdef PLATFORM_WINDOWS
    ZString tmp;
    if(absolute)
        tmp << drive << ':' << ZPATH_DELIM;
    for(unsigned i = 0; i < data.size(); ++i){
        tmp << data[i] << ZPATH_DELIM;
    }
    return tmp.popLast();
#else
    ZString tmp;
    if(absolute)
        tmp = ZPATH_DELIM;
    for(unsigned i = 0; i < data.size(); ++i){
        tmp << data[i] << ZPATH_DELIM;
    }
    return tmp.popLast();
#endif
}

unsigned ZPath::depth(){
    return size();
}
unsigned ZPath::size(){
    return data.size();
}

ArZ &ZPath::dat(){
    return data;
}
bool &ZPath::abs(){
    return absolute;
}
#ifdef PLATFORM_WINDOWS
char ZPath::drv(){
    return drive;
}

#endif

}
