/*******************************************************************************
**                                  LibChaos                                  **
**                                 zaccessor.h                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZACCESSOR_H
#define ZACCESSOR_H

#include "ztypes.h"

namespace LibChaos {

//! Interface for array index-accessible classes.
template <typename T> class ZAccessor {
public:
    virtual ~ZAccessor(){}

    virtual T &at(zu64 i) = 0;
    virtual const T &at(zu64 i) const  = 0;

    T &operator[](zu64 i){ return at(i); }
    const T &operator[](zu64 i) const { return at(i); }

    virtual T *raw() = 0;
    virtual const T *raw() const = 0;

    virtual zu64 size() const = 0;
};

}

#endif // ZACCESSOR_H
