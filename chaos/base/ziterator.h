/*******************************************************************************
**                                  LibChaos                                  **
**                                 ziterator.h                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZITERATOR_H
#define ZITERATOR_H

#include "ztypes.h"
#include "zaccessor.h"

namespace LibChaos {

template <typename T> class ZIterator {
public:
    ZIterator(ZAccessor<T> *iterator) : _iterator(iterator){

    }

    ~ZIterator(){
        delete _iterator;
    }

    bool atEnd() const {
        return _iterator->atEnd();
    }

    void advance(){
        _iterator->forward();
    }
    inline void operator++(){ advance(); }

    void recede(){
        _iterator->back();
    }
    inline void operator--(){ recede(); }

    virtual T &operator*(){
        return _iterator->current();
    }

private:
    ZAccessor<T> *_iterator;
};

}

#endif // ZITERATOR_H
