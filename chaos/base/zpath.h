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
    #define ZPATH_DEFAULT_DELIM '\\'
#else
    #define ZPATH_DEFAULT_DELIM '/'
#endif

namespace LibChaos {

class ZPath;
typedef ZArray<ZPath> ArP;

class ZPath {
public:
    ZPath();
    ZPath(ZString);
//    ZPath(std::string);
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

    // Determines if <path> is a child to <this>
    bool childTo(ZPath path) const; // Needs REWRITE

    ZPath &sanitize(); // Gets most direct path (resolves .. in path, as best as possible), removes unnecessary tokens
    static ZPath sanitize(ZPath path);

    static ZPath getAbsolute(ZPath path); // Get absolute representation of path, based on present working directory
    ZPath &getAbsolute(); // Operates on object

    // Hackish path repair crap, meant to make path system-specific valid
    bool valid();
    ZPath &fix();

    ZString str(ZString delim = ZPATH_DEFAULT_DELIM) const;

    // Data Accessors
    ArZ &data(){ return _data; }
    const ArZ &data() const { return _data; }
    bool &absolute(){ return _absolute; }
    const bool &absolute() const { return _absolute; }
    ZString &prefix(){ return _prefix; }
    const ZString &prefix() const { return _prefix; }

    // Size
    zu64 size() const { return _data.size(); }
    zu64 depth() const { return size(); }

private:
    void fromStr(ZString);
    static bool isDelim(char ch);

private:
//    const ArZ delimlist = { "/", "\\\\", "\\" };

    ArZ _data;
    bool _absolute;
    ZString _prefix;
    //ZString delim;
};

} // namespace LibChaos

#endif // ZPATH_H
