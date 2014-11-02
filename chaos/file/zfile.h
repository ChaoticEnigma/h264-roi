/*****************************************
**               LibChaos               **
**               zfile.h                **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZFILE_H
#define ZFILE_H

#include "zstring.h"
#include "zpath.h"
#include "zbinary.h"
#include "zreader.h"
#include "zwriter.h"

#if PLATFORM == WINDOWS
    #include <windows.h>
#else
    #include <fstream>
#endif

#define ZFILE_READ  ZFile::moderead
#define ZFILE_WRITE ZFile::modewrite

namespace LibChaos {

class ZFile : public ZReader, public ZWriter {
public:
    enum zfile_mode {
        moderead        = 0x001,    // Set to allow reading
        modewrite       = 0x002,    // Set to allow writing (implies create)
        modereadwrite   = 0x003,

        create          = 0x004,
        nocreate        = 0x008,    // Takes priority if create and nocreate set

        append          = 0x016,    // Takes priority if append and truncate set
        truncate        = 0x032,
    };

private:
    enum zfile_bits {
        readbit         = 0x001,    // Reading allowed
        writebit        = 0x002,    // Writing allowed
        readwritebits   = 0x003,

        createbit       = 0x004,    // Create allowed
        apptruncbit     = 0x008,    // Not set for append, set for truncate

        goodbit         = 0x064
    };

public:
    ZFile();
    ZFile(ZPath path, zfile_mode mode = moderead);
    ~ZFile();

    bool open(ZPath path);
    bool open(ZPath path, zfile_mode mode);
    void setMode(zfile_mode mode);
    bool close();

    // ZPosition
    zu64 getPos() const;
    zu64 setPos(zu64 pos);
    bool atEnd() const;
    // ZReader
    zu64 read(zbyte *dest, zu64 size);
    // ZWriter
    zu64 write(const zbyte *data, zu64 size);

    zu64 read(ZBinary &out, zu64 size);
    zu64 write(const ZBinary &data);

    bool remove();
    static bool remove(ZPath path);

    bool resizeFile(zu64 size);

    zu64 fileSize() const;
    static zu64 fileSize(ZPath path);

    bool isOpen() const {
#if PLATFORM == WINDOWS
        return (_file != NULL && _bits & goodbit);
#else
        return (_file != NULL && _bits & goodbit);
#endif
    }
    int &bits(){ return _bits; }
    ZPath path() const { return _path; }

#if PLATFORM == WINDOWS
    HANDLE handle(){ return _file; }
#else
    FILE *fp(){ return _file; }
#endif

    // Non-object
    static zu64 readBinary(ZPath name, ZBinary &out);
    static zu64 writeBinary(ZPath name, const ZBinary &data);

    static zu64 copy(ZPath src, ZPath dest);
    static bool rename(ZPath old, ZPath newfl);

    static bool removeDir(ZPath path);

    // Path-related functions
    static bool exists(ZPath path);

    static bool isFile(ZPath dir);
    static bool isDir(ZPath dir);

    static bool makeDir(ZPath dir);
    static bool createDirsTo(ZPath path);

    static ZArray<ZPath> listFiles(ZPath dir, bool recurse = true);
    static ZArray<ZPath> listDirs(ZPath dir, bool recurse = false);
    static zu64 dirSize(ZPath dir);

    static zu64 fileHash(ZPath path);

private:
    int _bits;
    ZPath _path;
#if PLATFORM == WINDOWS
    HANDLE _file;
#else
    FILE *_file;
#endif
};

} // namespace LibChaos

#endif // ZFILE_H
