/*****************************************
**               LibChaos               **
**               zfile.h                **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZFILE_H
#define ZFILE_H

#include <fstream>

#include "zstring.h"
#include "zpath.h"
#include "zbinary.h"

namespace LibChaos {

class ZFile {
public:
    enum zfile_mode {
        readonly    = 0x001,
        writeonly   = 0x002,
        readwrite   = 0x003,

        append      = 0x004,
        create      = 0x008,

        goodbit     = 0x064
    };

    ZFile();
    ZFile(ZPath, int = readonly);
    ~ZFile();

    bool open(ZPath, int = readonly);
    bool close();

    // does not work correctly
    zu64 read(ZBinary &out, zu64 size = -1);

    zu64 write(const char *);

    zu64 flsize();
    ZPath path(){ return _flpath; }

    bool isOpen();
    int getBits();

    FILE *fp(){
        return _fileh;
    }

    static ZString readFile(ZPath name);
    static ZString readFile(ZPath name, bool&);

    static ZBinary readBinary(ZPath name);

    static zu64 writeFile(ZPath name, const ZString &data);
    static zu64 writeFile(ZPath name, const ZBinary &data){ return writeBinary(name, data); }

    static zu64 writeBinary(ZPath name, const ZBinary &data);

    static zu64 copy(ZPath, ZPath);
    static bool createDirsTo(ZPath);

    //bool append(ZString);

    bool remove();
    static bool remove(ZPath);
    static bool removeDir(ZPath);

    static bool rename(ZPath old, ZPath newfl);

    bool exists();
    static bool exists(ZPath);

    // Path-related functions
    static bool isDir(ZPath dir);
    static bool isFile(ZPath dir);
    static ZArray<ZPath> listFiles(ZPath dir, bool recurse = true);
    static ZArray<ZPath> listDirs(ZPath dir, bool recurse = false);
    static zu64 dirSize(ZPath dir);

private:
    int _bits;
    ZPath _flpath;
    FILE *_fileh;
};

#define ZFILE_READ      ZFile::readonly
#define ZFILE_WRITE     ZFile::readwrite

} // namespace LibChaos

#endif // ZFILE_H
