/*******************************************************************************
**                                  LibChaos                                  **
**                                 ziterator.h                                **
**                          (c) 2015 Zennix Studios                           **
*******************************************************************************/
#ifndef ZITERATOR_H
#define ZITERATOR_H

#include "ztypes.h"

namespace LibChaos {

template <typename T> class ZIterator {
public:
    virtual ~ZIterator(){}

    virtual bool atEnd() const = 0;
    virtual void advance() = 0;

    virtual T &operator*() = 0;
};

}

#endif // ZITERATOR_H
