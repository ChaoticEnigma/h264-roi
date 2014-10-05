#ifndef ZWRITER_H
#define ZWRITER_H

#include "ztypes.h"

namespace LibChaos {

class ZWriter {
public:
    virtual ~ZWriter(){}

    virtual zu64 write(const zbyte *data, zu64 size) = 0;

    virtual bool atEnd() const = 0;
};

}

#endif // ZWRITER_H
