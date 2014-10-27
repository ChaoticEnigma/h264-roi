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
#include "zreader.h"
#include "zwriter.h"

namespace LibChaos {

class ZFile : public ZReader, public ZWriter {
public:
    enum zfile_mode {
        moderead        = 0x001,
        modewrite       = 0x002,
        modereadwrite   = 0x003,
    };

    enum zfile_bits {
        readbit         = 0x001,
        writebit        = 0x002,
        readwritebits   = 0x003,

        appendbit       = 0x004,
        createbit       = 0x008,

        goodbit         = 0x064
    };

    ZFile();
    ZFile(ZPath path, zfile_mode mode = moderead);
    ~ZFile();

    void setMode(zfile_mode mode);
    bool open(ZPath path);
    bool open(ZPath path, zfile_mode mode);
    bool close();

    // ZPosition
    zu64 getPos() const;
    void setPos(zu64 pos);
    bool atEnd() const;
    void rewind();

    // ZReader
    zu64 read(zbyte *dest, zu64 size);

    // ZWriter
    zu64 write(const zbyte *data, zu64 size);

    // does not work correctly
    zu64 read(ZBinary &out, zu64 size = (zu64)-1);

    zu64 flsize();
    ZPath path() const { return _flpath; }

    bool isOpen() const {
        return (_fileh != NULL && _bits & goodbit);
    }
    int &bits(){ return _bits; }

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

    static bool makeDir(ZPath dir);
    static bool createDirsTo(ZPath path);

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
