#ifndef ZREADER_H
#define ZREADER_H

#include "ztypes.h"

namespace LibChaos {

class ZReader {
public:
    virtual ~ZReader(){}

    virtual zu64 read(unsigned char *dest, zu64 length) = 0;

    virtual void rewind() = 0;

    virtual bool atEnd() const = 0;
    virtual void setReadPos(zu64 pos) = 0;
    virtual zu64 readPos() const = 0;
};

}

#endif // ZREADER_H
