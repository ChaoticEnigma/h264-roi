/*******************************************************************************
**                                  LibChaos                                  **
**                                zallocator.h                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZALLOCATOR_H
#define ZALLOCATOR_H

#include "ztypes.h"

#include <new>
#include <string.h>

namespace LibChaos {

template <class T> class ZAllocator {
public:
    // Allocates memory to hold <count> T's
    // Does not construct objects
    T *alloc(zu64 count){
        T *ptr = (T *) ::operator new(sizeof(T) * count, std::nothrow);
        if(ptr == nullptr)
            throw "Failed to alloc";
        return ptr;
    }

    // Deallocate memory
    // Expects <ptr> originally returned by alloc()
    // Objects should be destroy()ed first
    void dealloc(T *ptr){
        ::operator delete(ptr);
    }

    // Expects <ptr> originally returned by alloc()
    // Does not directly allocate memory, but constructors may
    // <ptr> must point to memory large enough to hold <count> T's
    T *construct(T *ptr, zu64 count = 1, T obj = T()){
        T *tmp = ptr;
        for(zu64 i = 0; i < count; ++i){
//            new (tmp++) T(obj);
            new (&(tmp[i])) T(obj);
        }
        return ptr;
    }

    // Expects <ptr> originally returned by alloc() and construct()ed
    // Does not dealloc()ate memory
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
        // TODO: ZAlocator move
        for(zu64 i = 0; i < count; ++i){
//            new (dest++) T(*src++);
            new (&(dest[i])) T(src[i]);
            destroy(src);
        }
    }

    static void memcopy(const void *src, void *dest, zu64 size){
        memcpy(dest, src, size);
    }

public:
    class RAW {
    public:
        static void *setRaw(void *dest, zu8 src, zu64 size){
            if(dest && size)
                memset((unsigned char *)dest, src, size);
            return dest;
        }

        static void *fillRaw(void *dest, const void *src, zu64 src_size, zu64 dest_count){
            for(zu64 i = 0; i < dest_count; ++i){
                memcpy((unsigned char *)dest + i, src, src_size);
            }
            return dest;
        }
    };
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
