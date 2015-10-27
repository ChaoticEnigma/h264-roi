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

    //! Check if there is another element after the current element.
    virtual bool more() const = 0;

    //! Move the iterator to the next element.
    virtual void advance() = 0;
    //! Prefix increment.
    inline T &operator++(){ advance(); return get(); }
    //! Postfix increment.
    inline T &operator++(int){ T &ref = get(); advance(); return ref; }
};

template <typename T> class ZDuplexIterator  : public ZSimplexIterator<T> {
public:
    virtual ~ZDuplexIterator(){}

    typename ZIterator<T>::iterator_type iteratorType() const { return ZIterator<T>::DUPLEX; }

    //! Get the current element.
    virtual T &get() = 0;

    //! Check if there is another element before the current element.
    virtual bool less() const = 0;

    //! Move the iterator to the previous element.
    virtual void recede() = 0;
    //! Prefix decrement.
    inline T &operator--(){ recede(); return get(); }
    //! Postfix decrement.
    inline T &operator--(int){ T &ref = get(); recede(); return ref; }
};

template <typename T> class ZRandomIterator  : public ZDuplexIterator<T> {
public:
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
