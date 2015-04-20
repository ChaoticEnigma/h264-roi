/*******************************************************************************
**                                  LibChaos                                  **
**                                 zaccessor.h                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZACCESSOR_H
#define ZACCESSOR_H

#include "ztypes.h"

namespace LibChaos {

template <typename T> class ZAccessor {
public:
    virtual ~ZIteratorAccessor(){}

    virtual bool atEnd() const = 0;
    virtual void forward() = 0;
    virtual void back() = 0;

    virtual T &operator*() = 0;
};

}

#endif // ZACCESSOR_H
