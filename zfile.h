#ifndef ZFILE_H
#define ZFILE_H

#include "zstring.h"
#include "zpath.h"
#include <fstream>
#include <sys/stat.h>

namespace LibChaos {

#define READ 1001
#define WRITE 1002
#define READWRITE 1003

class ZFile {
    public:
        ZFile();
        ZFile(ZString);
        ZFile(int);
        ZFile(int, ZString);
        ~ZFile();
        bool open(int, ZString);
        bool open(ZString);
        void close();
        ZString read();
        ZString read(unsigned);
        ZString readline();
        static ZString readFile(ZString name);
        static ZString readFile(ZPath name);
        static ZString readFile(ZString name, bool&);
        static ZString readFile(ZPath name, bool&);
        static bool writeFile(ZString name, ZString &data);
        static bool writeFile(ZPath name, ZString &data);
        bool append(ZString);
        bool write(ZString);
        bool remove();
        static bool remove(ZString);
        static bool remove(ZPath);
        bool exists();
        static bool exists(ZString);
        bool isOpen();
        bool good();
    private:
        void setMode(int);
        bool opened;
        bool readable;
        bool writeable;
        ZString filename;
        std::fstream file;
};

} // namespace LibChaos

#endif // ZFILE_H
