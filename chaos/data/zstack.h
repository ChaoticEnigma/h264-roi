#ifndef ZSTACK_H
#define ZSTACK_H

#include "ztypes.h"
#include "zarray.h"

namespace LibChaos {

template <typename T> class ZStack : private ZArray<T> {
public:
    ZStack(){}

    ZStack &push(const T &data){
        ZArray::push(data);
        return *this;
    }
    T pop(){
        T tmp = ZArray.back();
        ZArray::popBack();
        return tmp;
    }
    T &peek(){
        return ZArray::back();
    }
};

}

#endif // ZSTACK_H
