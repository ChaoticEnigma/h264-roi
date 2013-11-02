#ifndef ZFILE_H
#define ZFILE_H

#include <fstream>

#include "zstring.h"
#include "zpath.h"
#include "zbinary.h"

namespace LibChaos {

// First bit: read
// Second bit: write
#define ZFILE_READ      0x10
#define ZFILE_WRITE     0x01

class ZFile {
    public:
        ZFile();
        ZFile(ZPath, int = 0x10);
        ~ZFile();

        bool open(ZPath, int = 0x10);
        bool close();

        zu64 read(ZBinary &out, zu64 = -1);

        zu64 write(const char *);

        //ZString readline();

        static ZString readFile(ZPath name);
        static ZString readFile(ZPath name, bool&);

        static bool writeFile(ZPath name, const ZString &data);
        static zu64 writeFile(ZPath name, const ZBinary &data);

        //bool append(ZString);

        bool remove();
        static bool remove(ZPath);

        bool exists();
        static bool exists(ZPath);

        static ZArray<ZPath> listFiles(ZPath dir);

        zu64 flsize();

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

} // namespace LibChaos

#endif // ZFILE_H
