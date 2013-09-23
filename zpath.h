#ifndef ZPATH_H
#define ZPATH_H

#include <unistd.h>
#include <sys/stat.h>
//#include "asar.h"
#include "zarray.h"
#include "zstring.h"

namespace LibChaos {

class ZPath {
public:
    ZPath();
    ZPath(const char *);
    ZPath(std::string);
    ZPath(ZString);
    ZPath(ZString, bool);

    //ZPath &operator=(ZString);
    ZPath &operator=(ZPath);
    ZPath &operator=(ArZ);
    bool operator==(ZPath);
    ZPath operator+(ZPath);
    ZPath &operator<<(ZPath);
    ZString &operator[](unsigned);

    ZString &last();

    static ZPath pwd();
    ZPath &relTo(ZPath);
    ZPath parent();
    bool childTo(ZPath);
    ZPath &sanitize();
    ZPath &getAbs();
    static ZPath getAbs(ZPath path);
    ZPath &concat(ZPath);
    bool valid();
    ZPath &fix();
    static bool makeDir(ZPath);
    bool createDirsTo();
    ZString str();
    bool &abs();
    unsigned depth();
    unsigned size();
    ArZ &dat();
private:
    ArZ data;
    bool absolute;
};

} // namespace LibChaos

#endif // ZPATH_H
