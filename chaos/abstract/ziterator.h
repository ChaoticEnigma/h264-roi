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

public:
    virtual ~ZIteratorBase(){}
    virtual iterator_type iteratorType() const = 0;
};

// ////////////////////////////////////////
// ITERATOR
// ////////////////////////////////////////

template <typename T> class ZConstIterator : public virtual ZIteratorBase {
public:
    virtual ~ZConstIterator(){}

    //! Get the current const element.
    virtual const T &get() const = 0;
    inline const T &operator*() const { return get(); }
};

template <typename T> class ZIterator : public virtual ZConstIterator<T> {
public:
    virtual ~ZIterator(){}

    //! Get the current element.
    virtual T &get() = 0;
    inline T &operator*(){ return get(); }

    //! Get the current const element.
    virtual const T &get() const = 0;
    inline const T &operator*() const { return get(); }
};

// ////////////////////////////////////////
// SIMPLEX ITERATOR
// ////////////////////////////////////////

template <typename T> class ZSimplexConstIterator : public virtual ZConstIterator<T> {
public:
    virtual ~ZSimplexConstIterator(){}
    ZIteratorBase::iterator_type iteratorType() const override { return ZIteratorBase::SIMPLEX; }

    //! Check if there is another element after the current element.
    virtual bool more() const = 0;

    //! Move the iterator to the next element.
    virtual void advance() = 0;
    //! Prefix increment.
    inline void operator++(){ advance(); }
    //! Postfix increment.
    inline void operator++(int){ advance(); }
};

template <typename T> class ZSimplexIterator : public virtual ZIterator<T>, public virtual ZSimplexConstIterator<T> {
public:
    virtual ~ZSimplexIterator(){}
};

// ////////////////////////////////////////
// DUPLEX ITERATOR
// ////////////////////////////////////////

template <typename T> class ZDuplexConstIterator  : public virtual ZSimplexConstIterator<T> {
public:
    virtual ~ZDuplexConstIterator(){}
    ZIteratorBase::iterator_type iteratorType() const override { return ZIteratorBase::DUPLEX; }

    //! Check if there is another element before the current element.
    virtual bool less() const = 0;

    //! Move the iterator to the previous element.
    virtual void recede() = 0;
    //! Prefix decrement.
    inline void operator--(){ recede(); }
    //! Postfix decrement.
    inline void operator--(int){ recede(); }
};

template <typename T> class ZDuplexIterator  : public virtual ZSimplexIterator<T>, public virtual ZDuplexConstIterator<T> {
public:
    virtual ~ZDuplexIterator(){}
    ZIteratorBase::iterator_type iteratorType() const override { return ZIteratorBase::DUPLEX; }
};

// ////////////////////////////////////////
// RANDOM ITERATOR
// ////////////////////////////////////////

template <typename T> class ZRandomConstIterator  : public virtual ZDuplexConstIterator<T> {
public:
    virtual ~ZRandomConstIterator(){}
    ZIteratorBase::iterator_type iteratorType() const override { return ZIteratorBase::RANDOM; }

    //! Get number of elements.
    virtual zu64 size() const = 0;

    //! Get element at index.
    virtual T &at(zu64 i) = 0;
    inline T &operator[](zu64 i){ return at(i); }

    //! Get const element at index.
    virtual const T &at(zu64 i) const  = 0;
    inline const T &operator[](zu64 i) const { return at(i); }
};

template <typename T> class ZRandomIterator  : public virtual ZDuplexIterator<T>, public virtual ZRandomConstIterator<T> {
public:
    virtual ~ZRandomIterator(){}
    ZIteratorBase::iterator_type iteratorType() const override { return ZIteratorBase::RANDOM; }
};

}

#endif // ZITERATOR_H
