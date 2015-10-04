/*******************************************************************************
**                                  LibChaos                                  **
**                                 ziterator.h                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZITERATOR_H
#define ZITERATOR_H

#include "ztypes.h"
#include "ziterable.h"

namespace LibChaos {

template <class T> class ZIterator : public ZIterable<T> {
public:
    T &get() = delete;
    void advance() = delete;
    void recede() = delete;
    bool atEnd() = delete;
};

}

#endif // ZITERATOR_H
