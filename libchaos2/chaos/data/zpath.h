/*****************************************
**               LibChaos               **
**               zpath.h                **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZPATH_H
#define ZPATH_H

#include "zarray.h"
#include "zstring.h"

#if PLATFORM == WINDOWS
    #define ZPATH_DEFAULT_DELIM "\\"
#else
    #define ZPATH_DEFAULT_DELIM "/"
#endif

namespace LibChaos {

class ZPath;
typedef ZArray<ZPath> ArP;

class ZPath {
public:
    ZPath();
    ZPath(ZString);
    ZPath(std::string);
    ZPath(const char *);
    ZPath(ArZ arr);

    bool operator==(ZPath);
    inline bool operator!=(ZPath pth){
        return !(operator==(pth));
    }

    ZPath &concat(ZPath);
    ZPath &operator<<(ZPath);
    ZPath operator+(ZPath);

    ZString &operator[](unsigned);
    ZString &last();

    static ZPath pwd(); // Get a ZPath that represents the present working directory of the program

    ZPath &relativeTo(ZPath absolute); // Get a path from an absolute path that is a relative path to the same location, relative to <path>
    static ZPath relativeTo(ZPath path, ZPath absolute);

    ZPath &parent(); // Get the path to the next directory up
    static ZPath parent(ZPath path);

    bool childTo(ZPath); // Needs REWRITE

    ZPath &sanitize(); // Gets most direct path (resolves .. in path, as best as possible), removes unnecessary tokens
    static ZPath sanitize(ZPath path);

    static ZPath getAbsolute(ZPath path); // Get absolute representation of path, based on present working directory
    ZPath &getAbsolute(); // Operates on object

    bool valid();
    ZPath &fix();
    static bool makeDir(ZPath);
    bool createDirsTo();

    ZString str(ZString delim = ZPATH_DEFAULT_DELIM) const;

    ArZ &data();

    bool &absolute();

    zu64 size() const {
        return _data.size();
    }
    zu64 depth() const {
        return size();
    }

private:
    void fromStr(ZString);

private:
//    const ArZ delimlist = { "/", "\\\\", "\\" };

    ArZ _data;
    bool _absolute;
    ZString prefix;
    //ZString delim;
};

} // namespace LibChaos

#endif // ZPATH_H
