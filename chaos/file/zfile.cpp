#include "zfile.h"

#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>

#include "zlog.h"
#include "zerror.h"

namespace LibChaos {

ZFile::ZFile() : _bits(0){}
ZFile::ZFile(ZPath name, int mode) : _bits(0){
    open(name, mode);
}
ZFile::~ZFile(){
    close();
}

bool ZFile::open(ZPath path, int mode){
    _flpath = path;
    _bits = mode;

    // Close previous file
    close();

    // If we're not allowed to create and write
    if(!((_bits & create) && (_bits & writeonly))){
        // Check if the file exists
        if(!(exists(_flpath) && isFile(_flpath))){
            // Fail if it doesn't
            return false;
        }
    }

    // Set flags
    ZString modech;
    if(_bits & readwrite){ // read / write
        modech += "r+";
    } else if(_bits & readonly){ // read
        modech += "r";
    } else if(_bits & writeonly){ // write
        modech += "w";
    }
    modech += "b"; // binary

    _fileh = fopen(_flpath.str().cc(), modech.cc());
    if(_fileh != NULL){
        _bits = _bits | goodbit;
        return true;
    }
    return false;
}

bool ZFile::close(){
    if(isOpen())
        return fclose(_fileh) == 0;
    else
        return false;
}

zu64 ZFile::read(ZBinary &out, zu64 max){
    if(!(isOpen() && (_bits & readonly)))
        return 0;
    unsigned char *buffer;
    if(flsize() >= max)
        buffer = new unsigned char[max];
    else
        buffer = new unsigned char[flsize()];
    //zu64 dats = fread(buffer, 1, sizeof buffer, _fileh);
    zu64 len = fread(buffer, 1, sizeof buffer, _fileh);
    out = ZBinary(buffer, len);
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
        buffer.resize((unsigned long)infile.tellg());
        infile.seekg(0, std::ios::beg);
        infile.read(&buffer[0], (long)buffer.size());
        infile.close();
        status = true;
        return(ZString(buffer));
    }
    status = false;
    return ZString();
}

ZBinary ZFile::readBinary(ZPath file){
    struct stat st_buf;
    int ret = stat(file.str().cc(), &st_buf);
    if(ret != 0)
        throw ZError("ZFile: stat error");
    if(S_ISDIR(st_buf.st_mode))
        throw ZError("ZFile: file is directory");

    FILE *fp = fopen(file.str().cc(), "rb");
    if(fp == NULL)
        throw ZError("ZFile: fopen error");

    fseek(fp, 0, SEEK_END);
    zu64 size = (zu64)ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char *buffer = new (std::nothrow) unsigned char[size];
    if(buffer == nullptr)
        throw ZError("ZFile: new alloc error");

    zu64 len = fread(buffer, 1, size, fp);
    fclose(fp);
    if(len != size)
        throw ZError("ZFile: fread error");

    ZBinary data(buffer, size);
    delete[] buffer;
    return data;
}

zu64 ZFile::writeFile(ZPath filenm, const ZString &str){
    return writeFile(filenm, ZBinary(str.cc(), str.size()));
}

zu64 ZFile::writeBinary(ZPath file, const ZBinary &data){
    if(!file.createDirsTo())
        return 0;

    FILE *fp = fopen(file.str().cc(), "wb");
    if(fp == NULL)
        throw ZError("fopen error");

    zu64 wrt = fwrite(data.raw(), sizeof(char), data.size(), fp);
    fclose(fp);
    if(wrt != data.size())
        throw ZError("fwrite error");

    return wrt;
}

zu64 ZFile::copy(ZPath source, ZPath output){
    FILE* inFile = fopen(source.str().cc(), "rb");
    FILE* outFile = fopen(output.str().cc(), "wb");
    zu64 total = 0;
    char *buffer = new char[32768];
    zu64 dats;
    do {
        dats = fread(buffer, 1, sizeof(buffer), inFile);
        if(dats <= 0)
            break;
        total += fwrite(buffer, 1, dats, outFile);
    } while(dats == sizeof(buffer));
    delete[] buffer;
    fclose(outFile);
    fclose(inFile);
    return total;
}

bool ZFile::createDirsTo(ZPath path){
    return path.createDirsTo();
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
//bool ZFile::removeDir(ZPath name){
//    const char *path = name.str().cc();
//    DIR *d = opendir(path);
//    size_t path_len = strlen(path);
//    int r = -1;

//    if(d){
//       struct dirent *p;
//       r = 0;
//       while(!r && (p=readdir(d))){
//           int r2 = -1;
//           char *buf;
//           size_t len;
//           /* Skip the names "." and ".." as we don't want to recurse on them. */
//           if(!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")){
//              continue;
//           }
//           len = path_len + strlen(p->d_name) + 2;
//           buf = (char *)malloc(len);

//           if(buf){
//              struct stat statbuf;
//              snprintf(buf, len, "%s/%s", path, p->d_name);
//              if (!stat(buf, &statbuf)){
//                 if(S_ISDIR(statbuf.st_mode)){
//                    r2 = ZFile::removeDir(ZPath(ZString(buf)));
//                 } else {
//                    r2 = unlink(buf);
//                 }
//              }
//              free(buf);
//           }
//           r = r2;
//       }
//       closedir(d);
//    }
//    if(!r){
//       r = rmdir(path);
//    }
//    return r;
//}

bool ZFile::removeDir(ZPath name) {
    using namespace std;
    string path = name.str().str();
    if (path[path.length()-1] != '\\') path += "\\";
    // first off, we need to create a pointer to a directory
    DIR *pdir = NULL; // remember, it's good practice to initialise a pointer to NULL!
    pdir = opendir (path.c_str());
    struct dirent *pent = NULL;
    if (pdir == NULL) { // if pdir wasn't initialised correctly
        return false; // return false to say "we couldn't do it"
    } // end if
    char file[256];

    int counter = 1; // use this to skip the first TWO which cause an infinite loop (and eventually, stack overflow)
    while((pent = readdir(pdir))) { // while there is still something in the directory to list
        if (counter > 2) {
            for (int i = 0; i < 256; i++) file[i] = '\0';
            strcat(file, path.c_str());
            if (pent == NULL) { // if pent has not been initialised correctly
                return false; // we couldn't do it
            } // otherwise, it was initialised correctly, so let's delete the file~
            strcat(file, pent->d_name); // concatenate the strings to get the complete path
            if(ZFile::isDir(ZString(file)) == true) {
                ZFile::removeDir(file);
            } else { // it's a file, we can use remove
                remove(file);
            }
        } counter++;
    }

    // finally, let's clean up
    closedir (pdir); // close the directory
    if(!rmdir(path.c_str()))
        return false; // delete the directory
    return true;
}

bool ZFile::rename(ZPath old, ZPath newfl){
    if(::rename(old.str().cc(), newfl.str().cc()) == 0)
        return true;
    return false;
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

bool ZFile::isDir(ZPath dir){
    struct stat st;
#if COMPILER == MINGW
    stat(dir.str().cc(), &st);
#elif COMPILER == GCC
    lstat(dir.str().cc(), &st);
#endif
    if(S_ISDIR(st.st_mode)){
        return true;
    }
    return false;
}
bool ZFile::isFile(ZPath file){
    if(!exists(file))
        return false;
    struct stat st;
#if COMPILER == MINGW
    stat(file.str().cc(), &st);
#elif COMPILER == GCC
    lstat(file.str().cc(), &st);
#endif
    if(S_ISREG(st.st_mode)){
        return true;
    }
    return false;
}

ZArray<ZPath> ZFile::listFiles(ZPath dir, bool recurse){
    ZArray<ZPath> files;
    if(!isDir(dir)){
        return ZArray<ZPath>(dir);
    }
    DIR *dr = opendir(dir.str().cc());
    if(dr != NULL){
        struct dirent *drnt;
        while((drnt = readdir(dr)) != NULL){
            if(ZString(drnt->d_name) == "." || ZString(drnt->d_name) == "..")
                continue;
            ZPath flnm = dir + drnt->d_name;
            struct stat st;
#if COMPILER == MINGW
            stat(flnm.str().cc(), &st);
#elif COMPILER == GCC
            lstat(flnm.str().cc(), &st);
#endif
            if(S_ISDIR(st.st_mode)){
                if(recurse)
                    files.concat(listFiles(flnm));
            } else {
                files.push(flnm.getAbsolute());
            }
        }
        closedir(dr);
    }
    return files;
}
ZArray<ZPath> ZFile::listDirs(ZPath dir, bool recurse){
    ZArray<ZPath> dirs;
    if(!isDir(dir))
        return dirs;
    DIR *dr;
    struct dirent *drnt;
    if((dr = opendir(dir.str().cc())) != NULL){
        while((drnt = readdir(dr)) != NULL){
            if(ZString(drnt->d_name) == "." || ZString(drnt->d_name) == "..")
                continue;
            ZPath flnm = dir + drnt->d_name;
            struct stat st;
#if COMPILER == MINGW
            stat(flnm.str().cc(), &st);
#elif COMPILER == GCC
            lstat(flnm.str().cc(), &st);
#endif
            if(S_ISDIR(st.st_mode)){
                dirs.push(flnm.getAbsolute());
                if(recurse){
                    dirs.concat(listDirs(flnm)); // Unsafe, stack overflow possibility
                }
            }
        }
        closedir(dr);
    }
    return dirs;
}

#if PLATFORM == WINDOWS
#define V 1
}
#include <windows.h>
namespace LibChaos {
#else
#define V 2
#endif

zu64 ZFile::dirSize(ZPath dir){
#if V == 1
    WIN32_FIND_DATAA data;
    zu64 total = 0;
    HANDLE sh = FindFirstFileA((dir + "*").str().cc(), &data);
    if(sh == INVALID_HANDLE_VALUE)
        return 0;
    do {
        if(std::string(data.cFileName) != "." && std::string(data.cFileName) != ".."){
            if((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
                total += dirSize(dir + data.cFileName);
            else
                //total += (zu64)(data.nFileSizeHigh * (MAXDWORD) + data.nFileSizeLow);
                total += data.nFileSizeLow | (zu64)data.nFileSizeHigh << 32;
        }
    } while(FindNextFileA(sh, &data));
    FindClose(sh);
    return total;
#elif V == 2
    ZArray<ZPath> fls = listFiles(dir);
    zu64 total = 0;
    for(zu64 i = 0; i < fls.size(); ++i){
        total += ZFile(fls[i]).flsize();
    }
    return total;
#elif V == 3
    WIN32_FIND_DATAA data;
    HANDLE h;
    zu64 total = 0;
    ZArray<ZPath> fls = listFiles(dir);
    for(zu64 i = 0; i < fls.size(); ++i){
        h = FindFirstFileA(fls[i].str().cc(), &data);
        if(h == INVALID_HANDLE_VALUE){
            total += 0;
        } else {
            total += data.nFileSizeLow | (zu64)data.nFileSizeHigh << 32;
        }
        FindClose(h);
    }
    return total;
#elif V == 4
    FILE *pFile = NULL;
    zu64 total = 0;
    ZArray<ZPath> fls = listFiles(dir);
    for(zu64 i = 0; i < fls.size(); ++i){
        fopen_s(&pFile, dir.str().cc(), "rb");
        fseek(pFile, 0, SEEK_END);
        total += ftell(pFile);
        fclose(pFile);
        pFile = NULL;
    }
    return total;
#elif V == 5
    FILE *pFile;
    zu64 total = 0;
    DIR *dr;
    struct dirent *drnt;
    if((dr = opendir(dir.str().cc())) != NULL){
        while((drnt = readdir(dr)) != NULL){
            if(std::string(drnt->d_name) == "." || std::string(drnt->d_name) == "..")
                continue;
            ZPath flnm = dir + drnt->d_name;
            struct stat st;
#ifdef COMPILER_MINGW
            stat(flnm.str().cc(), &st);
#else
            lstat(flnm.str().cc(), &st);
#endif
            if(S_ISDIR(st.st_mode)){
                total += dirSize(flnm); // Unsafe, stack overflow possibility
            } else {
                fopen_s(&pFile, dir.str().cc(), "rb");
                fseek(pFile, 0, SEEK_END);
                total += ftell(pFile);
                fclose(pFile);
            }
        }
        closedir(dr);
    }
    return total;
#else
    return 0;
#endif
#undef V
}

zu64 ZFile::flsize(){
    if(!isOpen())
        return 0;
    fseek(_fileh, 0, SEEK_END);
    zu64 flsz = (zu64)ftell(_fileh);
    fseek(_fileh, 0, SEEK_SET);
    return flsz;
}

bool ZFile::isOpen(){
    return _bits & goodbit;
}
int ZFile::getBits(){
    return _bits;
}

}
