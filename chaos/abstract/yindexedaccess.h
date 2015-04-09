/*******************************************************************************
**                                  LibChaos                                  **
**                              yindexedaccess.h                              **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef YINDEXEDACCESS_H
#define YINDEXEDACCESS_H

#include "ztypes.h"

namespace LibChaos {

template <typename T> class YIndexedAccess {
public:
    virtual ~YIndexedAccess(){}

    virtual T &operator[](zu64 index) = 0;
    virtual const T &operator[](zu64 index) const = 0;
};

}

#endif // YINDEXEDACCESS_H
