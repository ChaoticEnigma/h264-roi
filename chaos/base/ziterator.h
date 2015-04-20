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
private:
    ZIterator(ZAccessor *iterator) : _iterator(iterator){

    }

public:
    ~ZIterator(){
        delete _iterator;
    }

    bool atEnd() const = 0;

    void advance(){
        _iterator->forward();
    }
    inline void operator++(){ advance(); }

    void recede(){
        _iterator->back();
    }
    inline void operator--(){ recede(); }

    virtual T &operator*(){

    }

private:
    ZAccessor *_iterator;
};

}

#endif // ZITERATOR_H
