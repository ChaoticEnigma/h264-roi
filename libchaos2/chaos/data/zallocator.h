#ifndef ZALLOCATOR_H
#define ZALLOCATOR_H

#include "ztypes.h"

namespace LibChaos {

template <typename T> class ZAllocatorBase {
public:
    virtual T *alloc(zu64 count) = 0;
    virtual void free() = 0;
protected:
    T *_data;
};

template <typename T> class ZAllocator : public ZAllocatorBase<T> {
public:
    T *alloc(zu64 count){
        this->_data = new T[count];
        return this->_data;
    }
    void free(){
        delete[] this->_data;
    }
};

template <> class ZAllocator<void> : public ZAllocatorBase<void> {
public:
    void *alloc(zu64 count){
        _data = new zbyte[count];
        return (void*)_data;
    }
    void free(){
        delete[] (zbyte*)_data;
    }
};

}

#endif // ZALLOCATOR_H
