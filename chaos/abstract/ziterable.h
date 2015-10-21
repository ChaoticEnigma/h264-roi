/*******************************************************************************
**                                  LibChaos                                  **
**                                 yiterable.h                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ITERABLE_H
#define ITERABLE_H

#include "ztypes.h"

#define ZITERABLE_COMPARE_OVERLOADS(T) \
    inline bool operator==(T it) const { return compare(&it); } \
    inline bool operator!=(T it) const { return !compare(&it); }

namespace LibChaos {

class ZIteratorBase {
public:
    enum iterator_type {
        //! Unidirectional iterator.
        SIMPLEX,
        //! Bidirectional iterator.
        DUPLEX,
        //! Random access iterator.
        RANDOM,
    };
};

template <typename T, ZIteratorBase::iterator_type U = ZIteratorBase::SIMPLEX> class ZIterable {
public:
    virtual ~ZIterable(){}

    //! Get the current element.
    virtual T &get() = 0;
    inline T &operator*(){ return get(); }

    virtual void advance() = 0;
    //! Prefix increment.
    inline T &operator++(){ advance(); return get(); }
    //! Postfix increment.
    inline T &operator++(int){ T &ref = get(); advance(); return ref; }

    virtual void recede() = 0;
    //! Prefix decrement.
    inline T &operator--(){ recede(); return get(); }
    //! Postfix decrement.
    inline T &operator--(int){ T &ref = get(); recede(); return ref; }

    //! Check if iterator is currently at the last element.
    virtual bool atEnd() const = 0;
};

}

#endif // ITERABLE_H
