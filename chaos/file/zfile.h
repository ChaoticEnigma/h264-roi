/*******************************************************************************
**                                  LibChaos                                  **
**                                  zfile.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZFILE_H
#define ZFILE_H

#include "zstring.h"
#include "zpath.h"
#include "zbinary.h"
#include "zposition.h"
#include "zreader.h"
#include "zwriter.h"

#if PLATFORM == WINDOWS || PLATFORM == CYGWIN
    #define ZFILE_WINAPI
#endif

#ifndef ZFILE_WINAPI
    #include <fstream>
#endif

#define ZFILE_READ  ZFile::moderead
#define ZFILE_WRITE ZFile::modewrite

namespace LibChaos {

class ZFile : public ZPosition, public ZReader, public ZWriter {
public:
    enum zfile_mode {
        READ        = 0x01,    //!< Set to allow reading
        WRITE       = 0x02,    //!< Set to allow writing (implies create)
        READWRITE   = 0x03,
        CREATE      = 0x04,
        NOCREATE    = 0x08,    //!< Takes priority if create and nocreate set
        APPEND      = 0x16,    //!< Takes priority if append and truncate set
        TRUNCATE    = 0x32,
    };

private:
    typedef void *HANDLE;

    enum zfile_bits {
        readbit         = 0x001,    //!< Reading allowed
        writebit        = 0x002,    //!< Writing allowed
        readwritebits   = 0x003,

        createbit       = 0x004,    //!< Create allowed
        apptruncbit     = 0x008,    //!< Not set for append, set for truncate
    };

public:
    ZFile();
    ZFile(ZPath path, zu16 mode = READ);
    ZFile(const ZFile &);
    ~ZFile();

    bool open(ZPath path);
    bool open(ZPath path, zu16 mode);
    void setMode(zu16 mode);
    bool close();

    // ZPosition
    zu64 tell() const;
    zu64 seek(zu64 pos);
    bool atEnd() const;

    // ZReader
    zu64 available() const;
    zu64 read(zbyte *dest, zu64 size);

    // ZWriter
    zu64 write(const zbyte *data, zu64 size);

    zu64 read(ZBinary &out, zu64 size);
    zu64 write(const ZBinary &data);

    zu64 write(const ZString &str);

    bool remove();
    static bool remove(ZPath path);

    bool resizeFile(zu64 size);

    zu64 fileSize() const;
    static zu64 fileSize(ZPath path);

#ifdef ZFILE_WINAPI
    bool isOpen() const { return (_handle != NULL); }
#else
    bool isOpen() const { return (_file != NULL); }
#endif
    zu16 &bits(){ return _options; }
    ZPath path() const { return _path; }

#ifdef ZFILE_WINAPI
    HANDLE handle(){ return _handle; }
#else
    FILE *fp(){ return _file; }
#endif

    // Non-object
    static zu64 readBinary(ZPath name, ZBinary &out);
    static zu64 writeBinary(ZPath name, const ZBinary &data);

    static ZString readString(ZPath path);

    static zu64 copy(ZPath src, ZPath dest);
    static bool rename(ZPath old, ZPath newfl);

    // Path-related functions
    static bool exists(ZPath path);

    static bool isFile(ZPath dir);
    static bool isDir(ZPath dir);

    //! Creates directory if it doesn't exist.
    static bool makeDir(ZPath dir);
    //! Creates all directories in path before last path part, if they don't exist.
    static bool createDirsTo(ZPath path);

    //! Removes a directory if it exists.
    static bool removeDir(ZPath path);

    //! List files in a directory, recursize by default.
    static ZArray<ZPath> listFiles(ZPath dir, bool recurse = false);
    //! List directories in a directory, non-recursive by default.
    static ZArray<ZPath> listDirs(ZPath dir, bool recurse = false, bool hidden = true);

    static zu64 dirSize(ZPath dir);

    static zu64 fileHash(ZPath path);

private:
    zu16 _options;
    ZPath _path;
#ifdef ZFILE_WINAPI
    HANDLE _handle;
#else
    FILE *_file;
#endif
};

} // namespace LibChaos

#endif // ZFILE_H
