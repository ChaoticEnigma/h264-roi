#ifndef ZWRITER_H
#define ZWRITER_H

#include "ztypes.h"
#include "zposition.h"

namespace LibChaos {

class ZWriter : public ZPosition {
public:
    virtual ~ZWriter(){}

    // Write <size> bytes from <src> to object at current position
    virtual zu64 write(const zbyte *src, zu64 size) = 0;
};

}

#endif // ZWRITER_H
