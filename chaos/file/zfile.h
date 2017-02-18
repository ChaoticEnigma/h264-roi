/*******************************************************************************
**                                  LibChaos                                  **
**                                  zfile.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZFILE_H
#define ZFILE_H

#include "zstring.h"
#include "zpath.h"
#include "zpointer.h"
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

namespace LibChaos {

/*! Reference counted cross-platform file handle abstraction.
 *  Intentionally does not support an append mode.
 */
class ZFile : public ZPosition, public ZReader, public ZWriter {
public:
    enum zfile_special {
        REGULAR,
        STDIN,
        STDOUT,
        STDERR,
    };

    enum zfile_mode {
        READ        = 0x01,     //!< Set to allow reading.
        WRITE       = 0x02,     //!< Set to allow writing (implies create)
        READWRITE   = 0x03,     //!< Rand and write.
        NOCREATE    = 0x08,     //!< Overrides default behavior to create files.
        TRUNCATE    = 0x10,     //!< Truncate file if it exists.
    };

public:
    ZFile(zfile_special type = REGULAR);
    ZFile(ZPath path, zu16 mode = READ);
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
    bool isOpen() const { return (_data->handle != NULL); }
#else
    bool isOpen() const { return (_data->file != NULL); }
#endif
    zu16 &bits(){ return _data->options; }
    ZPath path() const { return _data->path; }

#ifdef ZFILE_WINAPI
    HANDLE handle(){ return _data->handle; }
#else
    FILE *fp(){ return _data->file; }
#endif

public:
    //! Read contents of file at \a path into \a out.
    static zu64 readBinary(ZPath path, ZBinary &out);
    //! Write contents of \a data to file at \a path.
    static zu64 writeBinary(ZPath path, const ZBinary &data);

    //! Read contents of file at \a path as string.
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

    //! Get system error code.
    static int getError();
    //! Get system error string.
    static ZString getErrorString();

private:
    typedef void *HANDLE;

    enum zfile_bits {
        readbit         = 0x001,
        writebit        = 0x002,
        readwritebits   = 0x003,
        createbit       = 0x004,
        truncbit        = 0x008,
    };

    struct ZFileData {
        zfile_special type;
        zu16 options;
        ZPath path;
#ifdef ZFILE_WINAPI
        HANDLE handle;
#else
        FILE *file;
#endif
    };

private:
    ZPointer<ZFileData> _data;
};

} // namespace LibChaos

#endif // ZFILE_H
