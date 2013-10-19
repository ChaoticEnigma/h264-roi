#include "zfile.h"

#include <sys/stat.h>

namespace LibChaos {

ZFile::ZFile() : _bits(0x000){}
ZFile::ZFile(ZPath name, int mode) : _bits(0x000){
    open(name, mode);
}
ZFile::~ZFile(){
    close();
}

bool ZFile::open(ZPath path, int mode){
    _flpath = path;
    _bits = _bits & 0x011;
    struct stat st_buf;
    if(stat(_flpath.str().cc(), &st_buf) != 0 || S_ISDIR(st_buf.st_mode))
        return false;

    close();

    ZString modech;
    if(mode & 0x11){ // read / write
        _bits = 0x011;
        modech << "r+";
    } else if(mode & 0x01){ //write
        _bits = 0x001;
        modech << "w";
    } else if(mode & 0x10){ //read
        _bits = 0x010;
        modech << "r";
    }
    modech << "b"; // binary
    _fileh = fopen(_flpath.str().cc(), modech.cc());
    if(_fileh != NULL)
        _bits = _bits | 0x100;
    return _fileh != NULL;
}

bool ZFile::close(){
    if(isOpen())
        return fclose(_fileh) == 0;
    else
        return false;
}

zu64 ZFile::read(char *data, zu64 max){
    if(!isOpen() || !(_bits & 0x010))
        return 0;
    char *buffer;
    if(flsize() >= max)
        buffer = new char[max];
    else
        buffer = new char[flsize()];
    //zu64 dats = fread(buffer, 1, sizeof buffer, _fileh);
    zu64 len = fread(buffer, 1, sizeof buffer, _fileh);
    data = buffer;
    //delete[] buffer;
    return len;
}

zu64 ZFile::write(const char *cont){
    if(!isOpen() || !(_bits & 0x001))
        return 0;
    return fwrite(cont, 1, strlen(cont), _fileh);
}

/*ZString ZFile::readline(){
    if(_file){
        std::string line;
        std::getline(_file, line);
        return ZString(line);
    }
    return ZString();
}*/

ZString ZFile::readFile(ZPath filenm){
    bool st;
    return readFile(filenm, st);
}
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

/*bool ZFile::append(ZString cont){
    if(writeable){
        if(_file.is_open()){
            _file.seekp(0, std::ios::end);
            _file << cont.cc();
            _file.flush();
            return true;
        }
    }
    return false;
}*/

bool ZFile::remove(){
    close();
    remove(_flpath);
    return true;
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
    return exists(_flpath);
}
bool ZFile::exists(ZPath name){
    if(FILE *file = fopen(name.str().cc(), "r")){
        fclose(file);
        return true;
    }
    return false;
}

zu64 ZFile::flsize(){
    if(!isOpen())
        return 0;
    fseek(_fileh, 0, SEEK_END);
    zu64 flsz = ftell(_fileh);
    rewind(_fileh);
    return flsz;
}

bool ZFile::isOpen(){
    return _bits & 0x100;
}
int ZFile::getBits(){
    return _bits;
}

}
