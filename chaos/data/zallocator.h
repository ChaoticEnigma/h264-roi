#ifndef ZALLOCATOR_H
#define ZALLOCATOR_H

#include "ztypes.h"

#include <new>
#include <string.h>

namespace LibChaos {

template <class T> class ZAllocator {
public:
    // Allocates memory to hold <count> T's
    T *alloc(zu64 count){
        T *ptr = (T *) ::operator new(sizeof(T) * count, std::nothrow);
        if(ptr == nullptr)
            throw "Failed to alloc";
        return ptr;
    }

    // Expects a pointer originally returned by alloc()
    void dealloc(T *ptr){
        ::operator delete(ptr);
    }

    // Expects <ptr> originally returned by alloc()
    // <ptr> must point to memory large enough to hold <count> T's
    // Does not allocate memory
    T *construct(T *ptr, zu64 count = 1, T obj = T()){
        T *tmp = ptr;
        for(zu64 i = 0; i < count; ++i){
//            new (tmp++) T(obj);
            new (&(tmp[i])) T(obj);
        }
        return ptr;
    }

    // Expects <ptr> originally returned by alloc() and construct()ed
    // Does not delete memory
    void destroy(T *ptr, zu64 count = 1){
        for(zu64 i = 0; i < count; ++i){
//            (ptr++)->~T();
            (&(ptr[i]))->~T();
        }
    }

    void rawcopy(const T *src, T *dest, zu64 count){
        memcpy(dest, src, sizeof(T) * count);
    }
    void rawmove(const T *src, T *dest, zu64 count){
        memmove(dest, src, sizeof(T) * count);
    }

    // Expects <src> originally returned by alloc() and construct()ed
    // <src> must point to <count> T's
    // <dest> must point to memory large enough to hold <count> T's
    void copy(const T *src, T *dest, zu64 count){
        for(zu64 i = 0; i < count; ++i){
//            new (dest++) T(*++);
            new (&(dest[i])) T(src[i]);
        }
    }

    void move(T *src, T *dest, zu64 count){
        // TODO ZAlocator move
        for(zu64 i = 0; i < count; ++i){
//            new (dest++) T(*src++);
            new (&(dest[i])) T(src[i]);
            destroy(src);
        }
    }

    static void memcopy(const void *src, void *dest, zu64 size){
        memcpy(dest, src, size);
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
