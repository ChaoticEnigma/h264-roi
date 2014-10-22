#ifndef YLINEARACCESS_H
#define YLINEARACCESS_H

#include "ztypes.h"

namespace LibChaos {

template <typename T> class YLinearAccess {
public:
    virtual ~YLinearAccess(){}

    virtual void push(T push) = 0;

    virtual T &peek() = 0;
    virtual const T &peek() const = 0;

    virtual T pop() = 0;
};

}

#endif // YLINEARACCESS_H
