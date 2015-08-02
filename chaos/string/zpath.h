/*******************************************************************************
**                                  LibChaos                                  **
**                                  zpath.h                                   **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZPATH_H
#define ZPATH_H

#include "zarray.h"
#include "zstring.h"
#include "zhash.h"

#if PLATFORM == WINDOWS
    #define ZPATH_DEFAULT_DELIM '\\'
#else
    #define ZPATH_DEFAULT_DELIM '/'
#endif

namespace LibChaos {

//! Platform-aware path manipulation and storage.
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

    ZPath &append(ZPath path);
    ZPath &operator<<(ZPath path){ return append(path); }
    ZPath concat(ZPath path) const ;
    ZPath operator+(ZPath path) const { return concat(path); }

    ZString &operator[](zu64 index){ return _data[index]; }
    const ZString &operator[](zu64 index) const { return _data[index]; }
    ZString &last(){ return _data.back(); }
    const ZString &last() const { return _data.back(); }

    static ZPath pwd(); // Get a ZPath that represents the present working directory of the program

    ZPath &relativeTo(ZPath absolute); // Get a relative C to A (<this>) from B (<absolute>) such that B + C = A
    static ZPath relativeTo(ZPath path, ZPath absolute);

    ZPath &parent(); // Get the path to the next directory up
    static ZPath parent(ZPath path);

    // Determines if <path> is a child to <this>
    bool childTo(ZPath path) const; // Needs REWRITE

    ZPath &sanitize(); // Gets most direct path (resolves .. in path, as best as possible), removes unnecessary tokens
    static ZPath sanitize(ZPath path);

    static ZPath getAbsolute(ZPath path); // Get absolute representation of path, based on present working directory
    ZPath &getAbsolute(); // Operates on object

    ZString getExtension() const;

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
    bool isEmpty() const { return (size() == 0); }

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

//ZHASH_USER_SPECIALIAZATION(ZPath, (const ZPath &path), (), {
//    ZString str = path.str();
//    feedHash(str.bytes(), str.size());
//    doneHash();
//})

template <ZHashBase::hashMethod M> class ZHash<ZPath, M> : public ZHashMethod<M> {
public:
    ZHash(const ZPath &path) : ZHashMethod<M>(){
        ZString str = path.str();
        this->feedHash(str.bytes(), str.size());
        this->doneHash();
    }
};
template <> class ZHash<ZPath, ZHashBase::defaultHash> : public ZHashMethod<ZHashBase::defaultHash> {
public:
    ZHash(const ZPath &path) : ZHashMethod<ZHashBase::defaultHash>(){
       ZString str = path.str();
       this->feedHash(str.bytes(), str.size());
       this->doneHash();
    }
};

} // namespace LibChaos

#endif // ZPATH_H
