#ifndef ZREADER_H
#define ZREADER_H

#include "ztypes.h"

namespace LibChaos {

class ZReader {
public:
    virtual ~ZReader(){}

    // Read <size> bytes from object at current position into <dest>
    virtual zu64 read(zbyte *dest, zu64 size) = 0;
};

}

#endif // ZREADER_H
