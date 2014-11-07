#ifndef ZALLOCATOR_H
#define ZALLOCATOR_H

#include "ztypes.h"
#include <cstring>

namespace LibChaos {

template <class T> class ZAllocator {
public:
    T *alloc(zu64 count){
        return (T *) ::operator new(sizeof(T) * count);
    }
    void dealloc(T *data){
        ::operator delete(data);
    }
    T *construct(T *ptr, zu64 count = 1, const T &obj = T()){
        T *tmp = ptr;
        for(zu64 i = 0; i < count; ++i)
            new (tmp++) T(obj);
        return ptr;
    }
    void destroy(T *ptr, zu64 count = 1){
        for(zu64 i = 0; i < count; ++i)
            (ptr++)->~T();
    }

    void rawcopy(const void *src, void *dest, zu64 size){
        memcpy(dest, src, size);
    }
    void copy(const T *src, T *dest, zu64 count){
        for(zu64 i = 0; i < count; ++i)
            new (dest++) T(*src++);
    }
    void move(T *src, T *dest, zu64 count){
        for(zu64 i = 0; i < count; ++i){
            new (dest++) T(*src++);
            destroy(src);
        }
    }
};

template <> class ZAllocator<void> {
public:
    void *alloc(zu64 count){
        return new zbyte[count];
    }
    void dealloc(void *data){
        delete[] (zbyte *)data;
    }
};

}

#endif // ZALLOCATOR_H
