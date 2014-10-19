#ifndef ZWRITER_H
#define ZWRITER_H

#include "ztypes.h"
#include "zposition.h"

namespace LibChaos {

class ZWriter : public ZPosition {
public:
    virtual ~ZWriter(){}

    virtual zu64 write(const zbyte *data, zu64 size) = 0;
};

}

#endif // ZWRITER_H
