/*******************************************************************************
**                                  LibChaos                                  **
**                                 ziterator.h                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZITERATOR_H
#define ZITERATOR_H

#include "ztypes.h"

#define ZITERATOR_COMPARE_OVERLOADS(T) \
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

template <typename T, ZIteratorBase::iterator_type U = ZIteratorBase::SIMPLEX> class ZIterator {
public:
    virtual ~ZIterator(){}

    //! Get the current element.
    virtual T &get() = 0;
    inline T &operator*(){ return get(); }

    virtual void advance() = 0;
    //! Prefix increment.
    inline T &operator++(){ advance(); return get(); }
    //! Postfix increment.
    inline T &operator++(int){ T &ref = get(); advance(); return ref; }

    //! Check if iterator is currently at the last element.
    virtual bool atEnd() const = 0;
};

template <typename T, ZIteratorBase::DUPLEX> class ZIterator<T, ZIteratorBase::SIMPLEX> {
public:
    virtual ~ZIterator(){}

    virtual void recede() = 0;
    //! Prefix decrement.
    inline T &operator--(){ recede(); return get(); }
    //! Postfix decrement.
    inline T &operator--(int){ T &ref = get(); recede(); return ref; }
};

}

#endif // ZITERATOR_H
