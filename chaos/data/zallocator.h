/*******************************************************************************
**                                  LibChaos                                  **
**                                zallocator.h                                **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZALLOCATOR_H
#define ZALLOCATOR_H

#include "ztypes.h"

// For memcpy, memset
#include <string.h>
// For std::nothrow
#include <new>

namespace LibChaos {

struct zallocator_exception {
    zallocator_exception(const char *err) : what(err){}
    const char *what;
};

template <class T = void> class ZAllocator {
public:
    virtual ~ZAllocator(){}

    /*! Allocates memory to hold \p count T's.
     * Does not construct objects.
     */
    virtual T *alloc(zu64 count = 1){
        T *ptr = (T*)::operator new(sizeof(T) * count, std::nothrow);
        if(ptr == nullptr)
            throw zallocator_exception("Failed to alloc()");
        return ptr;
    }

    /*! Deallocate memory.
     *  Expects \p ptr originally returned by alloc().
     *  Objects should be destroy()ed first.
     */
    virtual void dealloc(T *ptr){
        ::operator delete(ptr);
    }

    /*! Expects \p ptr originally returned by alloc().
     *  Does not directly allocate memory, but constructors may.
     *  \p ptr must point to memory large enough to hold \p count T's.
     */
    virtual T *construct(T *ptr, const T &obj = T(), zu64 count = 1){
        T *tmp = ptr;
        for(zu64 i = 0; i < count; ++i){
//            new (tmp++) T(obj);
//            new (&(tmp[i])) T(obj);
            new (tmp + i) T(obj);
        }
        return ptr;
    }

    /*! Expects \p ptr originally returned by alloc() and construct()ed.
     *  Does not dealloc()ate memory.
     */
    virtual void destroy(T *ptr, zu64 count = 1){
        for(zu64 i = 0; i < count; ++i){
            //(ptr++)->~T();
            //(&(ptr[i]))->~T();
            (ptr + i)->~T();
        }
    }

    virtual void rawcopy(const T *src, T *dest, zu64 count = 1){
        memcpy((void *)dest, (const void *)src, sizeof(T) * count);
    }
    virtual void rawmove(const T *src, T *dest, zu64 count = 1){
        memmove((void *)dest, (const void *)src, sizeof(T) * count);
    }

    /*! Expects \p src originally returned by alloc() and construct()ed.
     *  Calls object copy constructor.
     *  \param src must point to at least \p count T's.
     *  \param dest must point to memory large enough to hold at least \p count T's.
     */
    virtual void copy(const T *src, T *dest, zu64 count = 1){
        for(zu64 i = 0; i < count; ++i){
//            new (dest++) T(*++);
//            new (&(dest[i])) T(src[i]);
            new (dest + i) T(src[i]);
        }
    }

    // TODO: ZAllocator move
    virtual void move(T *src, T *dest, zu64 count = 1){
        for(zu64 i = 0; i < count; ++i){
//            new (dest++) T(*src++);
//            new (&(dest[i])) T(src[i]);
            new (dest + i) T(src[i]);
            destroy(src);
        }
    }

    //! Zero the bytes of \p dest.
    static void zero(T *dest, zu64 count = 1){
        ::memset(dest, 0, count * sizeof(T));
    }

public:
    //! Copy block of memory with \p size from \p src to \p dest.
    static void memcopy(const void *src, void *dest, zu64 size){
        memcpy(dest, src, size);
    }

    //! Get address from reference.
    static inline T *addressOf(T &ref){
        //return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(ref)));
        return (T*)&(char &)ref;
    }
    //! Get const address from const reference.
    static inline const T *addressOf(const T &ref){
        return (const T*)&(const char &)ref;
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
