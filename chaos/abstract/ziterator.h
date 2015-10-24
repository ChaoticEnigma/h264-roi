/*******************************************************************************
**                                  LibChaos                                  **
**                                 ziterator.h                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZITERATOR_H
#define ZITERATOR_H

#include "ztypes.h"

#define ZITERATOR_COMPARE_OVERLOADS(T) \
    inline bool operator==(T it) const { return compare(it); } \
    inline bool operator!=(T it) const { return !compare(it); }

namespace LibChaos {

template <typename T> class ZIterator {
public:
    enum iterator_type {
        //! Unidirectional iterator.
        SIMPLEX,
        //! Bidirectional iterator.
        DUPLEX,
        //! Random access iterator.
        RANDOM,
    };

public:
    virtual ~ZIterator(){}

    virtual iterator_type iteratorType() const = 0;

    //! Get the current element.
    virtual T &get() = 0;
    inline T &operator*(){ return get(); }

    //virtual bool compare(const ZIterator<T> *it) const = 0;
    //inline bool operator==(const ZIterator<T> *it) const { return compare(); }
    //inline bool operator!=(const ZIterator<T> *it) const { return !compare(); }
};

template <typename T> class ZSimplexIterator : public ZIterator<T> {
public:
    virtual ~ZSimplexIterator(){}

    typename ZIterator<T>::iterator_type iteratorType() const { return ZIterator<T>::SIMPLEX; }

    //! Get the current element.
    virtual T &get() = 0;

    virtual void advance() = 0;
    //! Prefix increment.
    inline T &operator++(){ advance(); return get(); }
    //! Postfix increment.
    inline T &operator++(int){ T &ref = get(); advance(); return ref; }

    //! Check if iterator is currently at the last element.
    virtual bool atEnd() const = 0;
};

template <typename T> class ZDuplexIterator  : public ZSimplexIterator<T> {
public:
    virtual ~ZDuplexIterator(){}

    //! Get the current element.
    virtual T &get() = 0;

    typename ZIterator<T>::iterator_type iteratorType() const { return ZIterator<T>::DUPLEX; }

    virtual void recede() = 0;
    //! Prefix decrement.
    inline T &operator--(){ recede(); return get(); }
    //! Postfix decrement.
    inline T &operator--(int){ T &ref = get(); recede(); return ref; }
};

template <typename T> class ZRandomIterator  : public ZDuplexIterator<T> {
    virtual ~ZRandomIterator(){}

    typename ZIterator<T>::iterator_type iteratorType() const { return ZIterator<T>::RANDOM; }

    //! Get element at index.
    virtual T &at(zu64 i) = 0;
    inline T &operator[](zu64 i){ return at(i); }

    //! Get const element at index.
    virtual const T &at(zu64 i) const  = 0;
    inline const T &operator[](zu64 i) const { return at(i); }
};

}

#endif // ZITERATOR_H
