/*******************************************************************************
**                                  LibChaos                                  **
**                                 yiterable.h                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ITERABLE_H
#define ITERABLE_H

#include "ztypes.h"
#include "ziterator.h"

namespace LibChaos {

template <typename T> class ZIterable {
public:
    virtual ~ZIterable(){}

    ZIterator<T> iterator();
};

}

#endif // ITERABLE_H
