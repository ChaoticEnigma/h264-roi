#include "zfile.h"

namespace LibChaos {

ZFile::ZFile(){
    opened = false;
    readable = true;
    writeable = false;
}
ZFile::ZFile(ZString file){
    readable = true;
    writeable = false;
    open(file);
}
ZFile::ZFile(int mode){
    opened = false;
    setMode(mode);
}
ZFile::ZFile(int mode, ZString name){
    open(mode, name);
}

bool ZFile::open(int mode, ZString name){
    opened = false;
    setMode(mode);
    return open(name);
}

void ZFile::setMode(int mode){
    if(mode == READ){
        readable = true;
        writeable = false;
    } else if(mode == WRITE){
        readable = false;
        writeable = true;
    } else if(mode == READWRITE){
        readable = true;
        writeable = true;
    } else {
        readable = false;
        writeable = false;
    }
}

bool ZFile::open(ZString name){
    filename = name;
    struct stat st_buf;
    int ret = stat(filename.cc(), &st_buf);
    if(ret != 0 || S_ISDIR(st_buf.st_mode))
        return false;
    if(readable && !writeable){
        file.open(name.cc(), std::ios::in | std::ios::binary);
    } else if(writeable && !readable){
        file.open(name.cc(), std::ios::out);
    } else if(readable && writeable){
        file.open(name.cc(), std::ios::in | std::ios::binary | std::ios::out);
    }
    if(file)
        return true;
    return false;
}

void ZFile::close(){
    if(file)
        file.close();
}

ZFile::~ZFile(){
    close();
}

// v2
ZString ZFile::read(){
    if(file){
        unsigned oldpos = file.tellg();
        file.seekg(0, std::ios::end);
        unsigned endpos = file.tellg();
        file.seekg(oldpos, std::ios::beg);
        return read(endpos - oldpos);
    }
    return ZString();
}
ZString ZFile::read(unsigned len){
    if(file){
        //std::string contents;
        //contents.resize(len);
        char *cont = new char[len];
        file.read(cont, len);
        ZString data = cont;
        free(cont);
        return data;
    }
    return ZString();
}

ZString ZFile::readline(){
    if(file){
        std::string line;
        std::getline(file, line);
        return ZString(line);
    }
    return ZString();
}
/*ZString ZFile::readFile(ZString filenm){
    bool st;
    return readFile(ZPath(filenm), st);
}*/
ZString ZFile::readFile(ZPath filenm){
    bool st;
    return readFile(filenm, st);
}
/*ZString ZFile::readFile(ZString filenm, bool &status){
    return readFile(ZPath(filenm), status);
}*/
ZString ZFile::readFile(ZPath filenm, bool &status){
    struct stat st_buf;
    int ret = stat(filenm.str().cc(), &st_buf);
    if(ret != 0){
        status = false;
        return ZString();
    }
    if(S_ISDIR(st_buf.st_mode)){
        status = false;
        return ZString();
    }
    std::ifstream infile(filenm.str().cc(), std::ios::in | std::ios::binary);
    if(infile){
        std::string buffer;
        infile.seekg(0, std::ios::end);
        buffer.resize(infile.tellg());
        infile.seekg(0, std::ios::beg);
        infile.read(&buffer[0], buffer.size());
        infile.close();
        status = true;
        return(ZString(buffer));
    }
    status = false;
    return ZString();
}

/*bool ZFile::writeFile(ZString filenm, ZString &data){
    return writeFile(ZPath(filenm), data);
}*/
bool ZFile::writeFile(ZPath filenm, ZString &data){
    if(!filenm.createDirsTo())
        return false;
    std::ofstream outfile(filenm.str().cc(), std::ios::out);
    if(outfile){
        outfile.write(data.cc(), data.size());
        outfile.flush();
        outfile.close();
        return true;
    }
    return false;
}

bool ZFile::append(ZString cont){
    if(writeable){
        if(file.is_open()){
            file.seekp(0, std::ios::end);
            file << cont.cc();
            file.flush();
            return true;
        }
    }
    return false;
}

bool ZFile::write(ZString cont){
    if(writeable){
        if(file.is_open()){
            file << cont.cc();
            file.flush();
            return true;
        }
    }
    return false;
}

bool ZFile::remove(){
    close();
    remove(filename);
}
bool ZFile::remove(ZString file){
    return remove(ZPath(file));
}
bool ZFile::remove(ZPath file){
    if(exists(file.str())){
        if(std::remove(file.str().cc()) == 0){
            return true;
        } else {
            return false;
        }
    } else {
        return true;
    }
}

bool ZFile::exists(){
    return exists(filename);
}
bool ZFile::exists(ZString name){
    if(FILE * file = fopen(name.cc(), "r")){
        fclose(file);
        return true;
    }
    return false;
}

bool ZFile::isOpen(){
    return file.is_open();
}

bool ZFile::good(){
    return file.good();
}

}
