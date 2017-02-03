/*******************************************************************************
**                                  LibChaos                                  **
**                              ypushpopaccess.h                              **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef YLINEARACCESS_H
#define YLINEARACCESS_H

#include "ztypes.h"

namespace LibChaos {

// Can represent FIFO or FILO element access
// FIFO (First In First Out): Queue
// FILO (First In Last Out): Stack
template <typename T> class YPushPopAccess {
public:
    virtual ~YPushPopAccess(){}

    virtual void push(const T &item) = 0;

    virtual T &peek() = 0;
    virtual const T &peek() const = 0;

    virtual void pop() = 0;
};

}

#endif // YLINEARACCESS_H
