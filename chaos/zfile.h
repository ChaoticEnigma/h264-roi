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
            readonly = 0x10,
            readwrite = 0x01
        };

        ZFile();
        ZFile(ZPath, int = readonly);
        ~ZFile();

        bool open(ZPath, int = readonly);
        bool close();

        zu64 read(ZBinary &out, zu64 = -1);

        zu64 write(const char *);

        //ZString readline();

        static ZString readFile(ZPath name);
        static ZString readFile(ZPath name, bool&);

        static ZBinary readBinary(ZPath name);

        //static bool writeFile(ZPath name, const ZString &data);
        static zu64 writeFile(ZPath name, const ZBinary &data);

        static zu64 copy(ZPath, ZPath);
        static bool createDirsTo(ZPath);

        //bool append(ZString);

        bool remove();
        static bool remove(ZPath);

        bool exists();
        static bool exists(ZPath);

        // Path-related functions
        static bool isDir(ZPath dir);
        static bool isFile(ZPath dir);
        static ZArray<ZPath> listFiles(ZPath dir, bool recurse = true);
        static ZArray<ZPath> listDirs(ZPath dir, bool recurse = false);
        static zu64 dirSize(ZPath dir);

        zu64 flsize();
        ZPath path(){ return _flpath; }

        bool isOpen();
        int getBits();
    private:
        // First bit: open
        // Second bit: read
        // Third bit: write
        int _bits;
        ZPath _flpath;
        //std::fstream _file;
        FILE* _fileh;
};

#define ZFILE_READ      ZFile::readonly
#define ZFILE_WRITE     ZFile::readwrite

} // namespace LibChaos

#endif // ZFILE_H
