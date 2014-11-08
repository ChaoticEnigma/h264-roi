#ifndef ZQUEUE_H
#define ZQUEUE_H

#include "ztypes.h"
#include "zlist.h"

namespace LibChaos {

template <typename T> class ZQueue : private ZList<T> {
public:
    ZQueue(){}

    ZQueue &push(const T &data){
        ZList::pushBack(data);
        return *this;
    }
    T pop(){
        T tmp = ZList::front();
        ZList::popFront();
        return tmp;
    }
    T &peek(){
        return ZList::front();
    }
};

}

#endif // ZQUEUE_H
