/*******************************************************************************
**                                  LibChaos                                  **
**                                 zpointer.h                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZPOINTER_H
#define ZPOINTER_H

#include "ztypes.h"
#include "zexception.h"

namespace LibChaos {

// TEST: ZPointer NEEDS EXTENSIVE TESTING
/*! Shared pointer class.
 *  Wraps a pointer to an arbitrary object, deletes it when the last Pointer to it is destroyed. \n
 *  Note that all Pointers to the same object point to the *same object*, and can change the common object. \n
 *  \n
 *  Implementation: \n
 *  Each Pointer has a pointer to a struct that contains the data pointer and an integer with a references count. \n
 *  This same struct is shared by all the Pointers for the same object. \n
 *  When a new Pointers is copied or assigned a struct, the refernce count is incremented. \n
 *  When a Pointer is destroyed, the reference count is decremented. \n
 *  If a Pointer is the last reference to the object, the object is deleted when the Pointer is destroyed or re-assigned. \n
 */
template <typename T> class ZPointer {
public:
    //! Empty container.
    ZPointer() : _data(nullptr){}

    //! Take ownership of the pointer.
    ZPointer(T *ptr) : ZPointer(){
        if(ptr != nullptr){
            _data = new PointerData;
            _data->ptr = ptr;
            _data->count = 0;
            _increment();
        }
    }

    //! Acquire shared ownership of other contained pointer.
    // NOTE: Other ZPointer object is not modified, but the data struct is
    ZPointer(const ZPointer &other) : _data(other._data){
        _increment();
    }

    //! Release contained pointer, acquire shared ownership of other contained pointer.
    ZPointer &operator=(const ZPointer &other){
        release();
        _data = other._data;
        _increment();
        return *this;
    }

    ~ZPointer(){
        release();
    }

    /*! Release the shared pointer.
     *  If this is the only object with shared ownership of the pointer,
     *  delete it, otherwise decrement the reference count.
     */
    void release(){
        if(_data != nullptr){
            if(unique()){
                // Delete the object
                delete _data->ptr;
                delete _data;
            } else {
                // Decrement reference count
                _decrement();
            }
        }
        _data = nullptr;
    }

    //! Swap contained pointer with another shared pointer object.
    void swap(ZPointer &other){
        PointerData *data = _data;
        _data = other._data;
        other._data = data;
    }

    // Check if this is the only Pointer to the object
    bool unique() const {
        return (count() == 0);
    }

    // Count number of other Pointers to the object
    zu64 count() const {
        return (_data == nullptr ? 0 : _data->count - 1);
    }

    //! Get pointer to shared object.
    T *ptr() const {
        if(_data == nullptr)
            throw ZException("ZPointer ptr: attempt to get pointer from object without ownership");
        return _data->ptr;
    }
    inline T *operator->(){ return ptr(); }

private:
    void _increment(){
        if(_data != nullptr)
            ++(_data->count);
    }
    void _decrement(){
        if(_data != nullptr)
            --(_data->count);
    }

private:
    struct PointerData {
        // Wrapped pointer
        T *ptr;
        // Reference count
        zu64 count;
    };

    PointerData *_data;
};

}

#endif // ZPOINTER_H
