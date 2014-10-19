#ifndef ZREADER_H
#define ZREADER_H

#include "ztypes.h"
#include "zposition.h"

namespace LibChaos {

class ZReader : public ZPosition {
public:
    virtual ~ZReader(){}

    virtual zu64 read(zbyte *dest, zu64 length) = 0;
};

}

#endif // ZREADER_H
