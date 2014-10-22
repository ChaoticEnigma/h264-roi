#ifndef ZITERATOR_H
#define ZITERATOR_H

#include "ztypes.h"

namespace LibChaos {

template <typename T> class ZIterator {
public:
    virtual ~ZIterator(){}

    virtual bool hasNext() const = 0;
    virtual T &next() = 0;
};

}

#endif // ZITERATOR_H
