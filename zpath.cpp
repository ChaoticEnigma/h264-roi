#include "zpath.h"
#include "zfile.h"

namespace LibChaos {

ZPath::ZPath() : data(), absolute(false){}

ZPath::ZPath(ZString path) : data(){
    operator=(path);
}
ZPath::ZPath(ZString path, bool absl) : data(){
    operator=(path);
    absolute = absl;
}
ZPath::ZPath(const char *path) : data(){
    operator=(ZString(path));
}

ZPath &ZPath::operator=(ZPath path){
    //data = path.dat();
    data.clear();
    for(unsigned i = 0; i < path.size(); ++i){
        data.push(path[i]);
    }
    absolute = path.abs();
    return *this;
}

ZPath &ZPath::operator=(ZString str){
    if(str[0] == '/')
        absolute = true;
    else
        absolute = false;
    data = str.explode('/');
    data.clean();
    return *this;
}

ZPath &ZPath::operator=(ArZ arr){
    absolute = false;
    data = arr;
}

bool ZPath::operator==(ZPath path){
    if(path.depth() != data.size())
        return false;
    for(unsigned i = 0; i < path.size(); ++i){
        if(path[i] != data[i])
            return false;
    }
    return true;
}

ZPath ZPath::operator+(ZPath path){
    ZPath tmp = *this;
    tmp.concat(path);
    return tmp;
}

ZPath &ZPath::operator<<(ZPath path){
    concat(path);
    return *this;
}

ZString &ZPath::operator[](unsigned inx){
    return data[inx];
}

ZString &ZPath::last(){
    return data[data.size()-1];
}

ZPath ZPath::pwd(){
    size_t sz = 256;
    char ptr[sz];
    getcwd(ptr, sz);
    //ZString str = ptr;
    //ZString str = get_current_dir_name();
    //ZString str = getwd(NULL);
    return ZPath(ZString(ptr));
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

ZPath ZPath::parent(){
    if(size() > 0){
        ZPath tmp = *this;
        tmp.dat().pop(tmp.dat().size()-1);
        return tmp;
    } else {
        return ZPath();
    }
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

ZPath &ZPath::getAbs(){
    operator=(getAbs(*this));
    return *this;
}

ZPath ZPath::getAbs(ZPath path){
    if(!path.abs()){
        ZPath cwd = pwd();
        cwd.concat(path);
        cwd.abs() = true;
        return cwd;
    }
    return path;
}

ZPath &ZPath::concat(ZPath path){
    data.concat(path.dat());
    return *this;
}

bool ZPath::valid(){
    for(unsigned i = 0; i < data.size(); ++i){
        if(data[i].count("/") > 0)
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
    if(mkdir(dir.str().cc(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0){
        return true;
    }
    return false;
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
    ZString tmp;
    if(abs())
        tmp = "/";
    for(unsigned i = 0; i < data.size(); ++i){
        tmp << data[i] << "/";
    }
    return tmp.popLast();
}

bool &ZPath::abs(){
    return absolute;
}

unsigned ZPath::depth(){
    return data.size();
}
unsigned ZPath::size(){
    return depth();
}

ArZ &ZPath::dat(){
    return data;
}

}
