#ifndef ZPOSITION_H
#define ZPOSITION_H

#include "ztypes.h"

namespace LibChaos {

class ZPosition {
public:
    virtual ~ZPosition(){}

    virtual zu64 getPos() const = 0;
    virtual void setPos(zu64 pos) = 0;
    virtual bool atEnd() const = 0;
    virtual void rewind() = 0;
};

}

#endif // ZPOSITION_H
