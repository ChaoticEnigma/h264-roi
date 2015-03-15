#include "zfile.h"

#include "zlog.h"
#include "zexception.h"

#include <stdlib.h>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>

#if COMPILER == GCC || COMPILER == MINGW
    #include <dirent.h>
    #include <unistd.h>
#endif

#include "xxhash.h"

#if PLATFORM == WINDOWS
    #define V 1
    #include <windows.h>
#else
    #define V 2
#endif

#define ZFILE_COPY_BUFFER_SIZE 32768

namespace LibChaos {

ZFile::ZFile() : _options(0 | readbit), _handle(NULL){}

ZFile::ZFile(ZPath name, int mode) : ZFile(){
    open(name, mode);
}

ZFile::~ZFile(){
    close();
}

bool ZFile::open(ZPath path){
    // Close previous file, if any
    close();
    _path = path;

    // Must open for read or write or both
    if(!(_options & readbit) && !(_options & writebit))
        return false;

    // If we're not allowed to create
    if(!(_options & createbit)){
        // Make sure the file exists
        if(!isFile(_path)){
            // Fail if it doesn't
            return false;
        }
    }

#if PLATFORM == WINDOWS
    DWORD access = 0;
    if(_options & readbit)
        access |= GENERIC_READ;
    if(_options & writebit)
        access |= GENERIC_WRITE;

    DWORD share = 0;
    if(_options & readbit && !(_options & writebit))
        share = FILE_SHARE_READ; // Share read access if only reading

    DWORD create;
    if(_options & createbit){
        if(_options & apptruncbit)
            create = CREATE_ALWAYS; // Always create new empty file
        else
            create = OPEN_ALWAYS; // Always open a file, create if not exists
    } else {
        if(_options & apptruncbit)
            create = TRUNCATE_EXISTING; // Truncate exiting file
        else
            create = OPEN_EXISTING; // Open file only if it exists
    }

    DWORD attr = FILE_ATTRIBUTE_NORMAL;

    _handle = CreateFile(_path.str('\\').wstr().c_str(), access, share, NULL, create, attr, NULL);
    if(_handle == INVALID_HANDLE_VALUE){
        _handle = NULL;
        return false;
    }

    return true;
#else
    // Set flags
    ZString modech;
    if(_bits & readwritebits){ // read / write
        if(_bits & createbit)
            modech += "w+";
        else
            modech += "r+";
    } else if(_bits & readbit){ // read
        modech += "r";
    } else if(_bits & writebit){ // write
        modech += "w";
    }
    modech += "b"; // binary

    _file = fopen(_path.str().cc(), modech.cc());
    if(_file == NULL){
        return false;
    }

    return true;
#endif
}

bool ZFile::open(ZPath path, int mode){
    setMode(mode);
    return open(path);
}

void ZFile::setMode(int mode){
    _options = 0;
    if(mode & moderead){
        _options |= readbit;
    }

    if(mode & modewrite){
        if(mode & nocreate){
            _options |= writebit;
        } else {
            _options |= writebit;
            _options |= createbit;
        }
    }

    // If append not set and truncate set
    if(!(mode & append) && mode & truncate)
        _options |= apptruncbit;
}

bool ZFile::close(){
    if(!isOpen())
        return true;
#if PLATFORM == WINDOWS
    bool ret = CloseHandle(_handle) != 0;
#else
    bool ret = (fclose(_file) == 0);
#endif
    _handle = NULL;
    return ret;
}

// ZPosition
zu64 ZFile::getPos() const {
#if PLATFORM == WINDOWS
    // Move pointer by 0 to get current pos
    LARGE_INTEGER distance;
    distance.QuadPart = 0;
    LARGE_INTEGER newpos;
    SetFilePointerEx(_handle, distance, &newpos, FILE_CURRENT);
    return (zu64)newpos.QuadPart;
#else
    // Tell file pointer position
    long pos = ftell(_file);
    return (pos > 0 ? (zu64)pos : 0);
#endif
}

zu64 ZFile::setPos(zu64 pos){
#if PLATFORM == WINDOWS
    LARGE_INTEGER distance;
    distance.QuadPart = (long long)pos;
    LARGE_INTEGER newpos;
    SetFilePointerEx(_handle, distance, &newpos, FILE_BEGIN);
    return (zu64)newpos.QuadPart;
#else
    // Seek file pointer to position
    fseek(_file, (long)pos, SEEK_SET);
    return getPos();
 #endif
}
bool ZFile::atEnd() const {
#if PLATFORM == WINDOWS
    // Hack
    return getPos() >= fileSize();
#else
    // Check if file pointer is at end of file
    return feof(_file);
#endif
}

// ZReader
zu64 ZFile::read(zbyte *dest, zu64 size){
    // Check file is open and has read bit set
    if(!isOpen() || !(_options & readbit))
        return 0;
#if PLATFORM == WINDOWS
    DWORD read;
    bool ret = ReadFile(_handle, dest, size, &read, NULL) != 0;
    if(!ret)
        return 0;
    return (zu64)read;
#else
    return fread(dest, sizeof(zbyte), size, _file);
#endif
}

// ZWriter
zu64 ZFile::write(const zbyte *src, zu64 size){
    // Check file is open and has write bit set
    if(!isOpen() || !(_options & writebit))
        return 0;
#if PLATFORM == WINDOWS
    DWORD write;
    bool ret = WriteFile(_handle, src, size, &write, NULL) != 0;
    if(!ret)
        return 0;
    return (zu64)write;
#else
    return fwrite(src, sizeof(zbyte), size, _file);
#endif
}

zu64 ZFile::read(ZBinary &out, zu64 size){
    out.resize(size);
    return read(out.raw(), size);
}

zu64 ZFile::write(const ZBinary &data){
    return write(data.raw(), data.size());
}

zu64 ZFile::write(const ZString &str){
    return write((const zbyte *)str.cc(), str.size());
}

bool ZFile::remove(){
    close();
    remove(_path);
    return true;
}
bool ZFile::remove(ZPath file){
#if COMPILER == MSVC
    if(isFile(file))
        return DeleteFile(file.str().wstr().c_str()) != 0;
    return false;
#else
    if(exists(file.str())){
        if(std::remove(file.str().cc()) == 0){
            return true;
        } else {
            return false;
        }
    } else {
        return true;
    }
#endif
}

bool ZFile::resizeFile(zu64 size){
#if PLATFORM == WINDOWS
    LARGE_INTEGER dist;
    dist.QuadPart = size;
    LARGE_INTEGER pos;
    if(SetFilePointerEx(_handle, dist, &pos, FILE_BEGIN) == 0)
        return false;
    if(dist.QuadPart != pos.QuadPart)
        return false;
    if(SetEndOfFile(_handle) == 0)
        return false;
    return true;
#else
    int fd = fileno(_file);
    if(ftruncate64(fd, (long long)size) != 0)
        return false;
    return true;
#endif
}

zu64 ZFile::fileSize() const {
#if PLATFORM == WINDOWS
    // Check file is open and has read bit set
    if(!isOpen() || !(_options & readbit))
        return 0;
    LARGE_INTEGER lint;
    if(!GetFileSizeEx(_handle, &lint))
        return 0;
    return (zu64)lint.QuadPart;
#else
    if(!isOpen())
        return 0;
    fseek(_file, 0, SEEK_END);
    zu64 flsz = (zu64)ftell(_file);
    fseek(_file, 0, SEEK_SET);
    return flsz;
#endif
}
zu64 ZFile::fileSize(ZPath path){
    ZFile file(path);
    return file.fileSize();
}

zu64 ZFile::readBinary(ZPath file, ZBinary &out){
    if(isFile(file))
        throw ZException("ZFile: file is directory");

    FILE *fp = fopen(file.str().cc(), "rb");
    if(fp == NULL)
        throw ZException("ZFile: fopen error");

    fseek(fp, 0, SEEK_END);
    zu64 size = (zu64)ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char *buffer = new (std::nothrow) unsigned char[size];
    if(buffer == nullptr)
        throw ZException("ZFile: new alloc error");

    zu64 len = fread(buffer, 1, size, fp);
    fclose(fp);
    if(len != size)
        throw ZException("ZFile: fread error");

    out.write(buffer, size);
    delete[] buffer;
    return size;
}

zu64 ZFile::writeBinary(ZPath path, const ZBinary &data){
    if(!ZFile::createDirsTo(path))
        throw ZException("could not create dirs to file");

    ZFile file(path, ZFile::modewrite);
    if(!file.isOpen())
        return 0;
    zu64 wrt = file.write(data.raw(), data.size());
    return wrt;
}

ZString ZFile::readString(ZPath path){
    ZFile file(path);
    ZString str('0', file.fileSize());
    file.read((zbyte *)str.c(), str.size());
    file.close();
    return str;
}

zu64 ZFile::copy(ZPath source, ZPath output){
    FILE *inFile = fopen(source.str().cc(), "rb");
    FILE *outFile = fopen(output.str().cc(), "wb");
    if(inFile == NULL || outFile == NULL)
        return 0;

    zu64 total = 0;
    char *buffer = new char[ZFILE_COPY_BUFFER_SIZE];
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

bool ZFile::rename(ZPath old, ZPath newfl){
    if(::rename(old.str().cc(), newfl.str().cc()) == 0)
        return true;
    return false;
}

bool ZFile::removeDir(ZPath name){
#if COMPILER == MSVC
    //TODO: Win32 removeDir
    return false;
#else
    using namespace std;
    string path = name.str().str();
    if(path[path.length()-1] != '\\')
        path += "\\";

    DIR *pdir = opendir(path.c_str());
    if(pdir == NULL)
        return false;
    char file[256];

    struct dirent *pent = NULL;
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

    closedir(pdir); // close the directory
    if(!rmdir(path.c_str()))
        return false; // delete the directory
    return true;
#endif
}

bool ZFile::exists(ZPath name){
#if COMPILER == MSVC
    DWORD attr = GetFileAttributes(name.str('\\').wstr().c_str());
    return (attr != INVALID_FILE_ATTRIBUTES); // Just checks that there is something, anything at that path
#else
    if(FILE *file = fopen(name.str().cc(), "r")){
        fclose(file);
        return true;
    }
    return false;
#endif
}

bool ZFile::isFile(ZPath file){
#if COMPILER == MSVC
    DWORD attr = GetFileAttributes(file.str('\\').wstr().c_str());
    if(attr != INVALID_FILE_ATTRIBUTES)
        return !(attr & FILE_ATTRIBUTE_DIRECTORY) && !(attr & FILE_ATTRIBUTE_REPARSE_POINT); // Not a directory or link
    return false;
#else
    if(!exists(file))
        return false;
    struct stat flstat;
    if(stat(file.str().cc(), &flstat) == 0){
        if(flstat.st_mode & S_IFREG)
            return true;
    }
    return false;
#endif // COMPILER == MSVC
}

bool ZFile::isDir(ZPath dir){
#if COMPILER == MSVC
    DWORD attr = GetFileAttributes(dir.str('\\').wstr().c_str());
    if(attr != INVALID_FILE_ATTRIBUTES)
        return (attr & FILE_ATTRIBUTE_DIRECTORY);
    return false;
#else
    struct stat flstat;
    if(stat(dir.str().cc(), &flstat) == 0){
        if(flstat.st_mode & S_IFDIR)
            return true;
    }
    return false;
#endif // COMPILER == MSVC
}

bool ZFile::makeDir(ZPath dir){
#if COMPILER == MSVC
    if(exists(dir)){
        if(isDir(dir))
            return true;
        else
            return false;
    } else {
        return CreateDirectory(dir.str('\\').wstr().c_str(), NULL) != 0;
    }
#else
    struct stat flstat;
    int ret = stat(dir.str().cc(), &flstat);
    if(ret == 0){
        if(S_ISDIR(flstat.st_mode)){
            return true;
        } else {
            return false;
        }
    }
#if COMPILER == MINGW
    ret = mkdir(dir.str().cc());
#else // GCC
    ret = mkdir(dir.str().cc(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    return ret == 0;
#endif // COMPILER == MSVC
}

bool ZFile::createDirsTo(ZPath dir){
    if(!dir.depth())
        return false;
    dir.sanitize();
    ZPath current;
    current.absolute() = dir.absolute();
    current.prefix() = dir.prefix();
    for(zu64 i = 0; i < dir.size()-1; ++i){
        current.append(dir[i]);
        if(!makeDir(current))
            return false;
    }
    return true;
}

ZArray<ZPath> ZFile::listFiles(ZPath dir, bool recurse){
    ZArray<ZPath> files;
    if(!isDir(dir)){
        return files;
    }
#if PLATFORM == WINDOWS
    WIN32_FIND_DATA finddata;
    HANDLE find = FindFirstFile((dir + "*").str('\\').wstr().c_str(), &finddata);
    if(find == INVALID_HANDLE_VALUE){
        return files;
    }
    do {
        ZString name = ZString(finddata.cFileName);
        if(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
            if(recurse && name != "." && name != ".."){
                files.append(listFiles(dir + name, recurse));
            }
        } else {
            files.push(dir + name);
        }
    } while(FindNextFile(find, &finddata) != 0);

    return files;
#else
    DIR *dr = opendir(dir.str().cc());
    if(dr != NULL){
        struct dirent *drnt;
        while((drnt = readdir(dr)) != NULL){
            if(ZString(drnt->d_name) == "." || ZString(drnt->d_name) == "..")
                continue;
            ZPath flnm = dir + drnt->d_name;
            struct stat st;
            lstat(flnm.str().cc(), &st);
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
#endif // PLATFORM
}
ZArray<ZPath> ZFile::listDirs(ZPath dir, bool recurse, bool hidden){
    ZArray<ZPath> dirs;
    if(!isDir(dir))
        return dirs;
#if PLATFORM == WINDOWS
    WIN32_FIND_DATA finddata;
    HANDLE find = FindFirstFile((dir + "*").str('\\').wstr().c_str(), &finddata);
    if(find == INVALID_HANDLE_VALUE){
        return dirs;
    }
    do {
        if((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (hidden || !(finddata.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))){
            ZString name = ZString(finddata.cFileName);
            if(name != "." && name != ".."){
                dirs.push(dir + name);
                if(recurse){
                    dirs.append(listDirs(dir + name, recurse));
                }
            }
        }
    } while(FindNextFile(find, &finddata) != 0);

    return dirs;
#else
    DIR *dr;
    struct dirent *drnt;
    if((dr = opendir(dir.str().cc())) != NULL){
        while((drnt = readdir(dr)) != NULL){
            if(ZString(drnt->d_name) == "." || ZString(drnt->d_name) == "..")
                continue;
            ZPath flnm = dir + drnt->d_name;
            struct stat st;
            lstat(flnm.str().cc(), &st);
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
#endif // PLATFORM
}

zu64 ZFile::dirSize(ZPath dir){
#if V == 1
    // TEST: ZFile dirSize
    WIN32_FIND_DATA data;
    zu64 total = 0;
    HANDLE sh = FindFirstFile((dir + "*").str('\\').wstr().c_str(), &data);
    if(sh == INVALID_HANDLE_VALUE)
        return 0;
    do {
        if(ZString(data.cFileName) != "." && ZString(data.cFileName) != ".."){
            if((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
                total += dirSize(dir + ZString(data.cFileName));
            else
                total += (zu64)(data.nFileSizeHigh * (MAXDWORD) + data.nFileSizeLow);
                //total += data.nFileSizeLow | (zu64)data.nFileSizeHigh << 32;
        }
    } while(FindNextFile(sh, &data));
    FindClose(sh);
    return total;
#elif V == 2
    ZArray<ZPath> fls = listFiles(dir);
    zu64 total = 0;
    for(zu64 i = 0; i < fls.size(); ++i){
        total += ZFile(fls[i]).fileSize();
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
#else // COMPILER_MINGW
            lstat(flnm.str().cc(), &st);
#endif // COMPILER_MINGW
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
#else // V
    return 0;
#endif // V
}

zu64 ZFile::fileHash(ZPath path){
    ZFile file;
    if(!file.open(path))
        return 0;
    XXH64_state_t *state = XXH64_createState();
    XXH64_reset(state, 0);
    zbyte *buffer = new zbyte[4096];
    zu64 readsize;
    do {
        readsize = file.read(buffer, 4096);
        XXH64_update(state, buffer, readsize);
    } while(readsize == 4096);
    delete[] buffer;
    zu64 hash = XXH64_digest(state);
    XXH64_freeState(state);
    return hash;
}

}
