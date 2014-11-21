#ifndef ZSERIALIZER_H
#define ZSERIALIZER_H

#include "ztypes.h"
#include "zbinary.h"

namespace LibChaos {

class ZSerializer {
public:
    ~ZSerializer(){}

    virtual ZBinary serialize() const = 0;
    virtual void deserialize(const ZBinary &serial) = 0;

};

}

#endif // ZSERIALIZER_H
