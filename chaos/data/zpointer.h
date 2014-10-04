#ifndef ZPOINTER_H
#define ZPOINTER_H

#include "ztypes.h"

namespace LibChaos {

// Wraps a pointer to an arbitrary object, deletes it when the last Pointer to it is destroyed
// Note that all Pointers to the same object point to the *same object*, and can change the common object
// Basically a shared pointer

// Implementation:
// Each Pointer has a pointer to a struct that contains the data pointer and an integer with a references count
// This same struct is shared by all the Pointers for the same object
// When a new Pointers is copied or assigned a struct, the refernce count is incremented
// When a Pointer is destroyed, the reference count is decremented
// If a Pointer is the last reference to the object, the object is deleted when the Pointer is destroyed or re-assigned

// NEEDS EXTENSIVE TESTING

template <typename T> class ZPointer {
public:
    ZPointer() : _data(nullptr){

    }

    ZPointer(T *ptr) : ZPointer(){
        if(ptr != nullptr){
            _data = new PointerData;
            _data->ptr = ptr;
            _data->count = 0;
            increment();
        }
    }

    // Copy other pointer and count
    // Increment count
    ZPointer(ZPointer &other) : _data(other._data){
        increment();
    }

    ZPointer &operator=(ZPointer &other){
        release();
        _data = other._data;
        increment();
        return *this;
    }

    ~ZPointer(){
        release();
    }

    void reset(){
        release();
        _data = nullptr;
    }

    void swap(ZPointer &other){
        T *data = _data;
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

    // Get pointer
    T *ptr() const {
        return _data->ptr;
    }

    // Get reference
    T &get(){
        return *_data->ptr;
    }
    const T&get() const {
        return *_data->ptr;
    }

private:
    struct PointerData {
        // Wrapped pointer
        T *ptr;
        // Reference count
        zu64 count;
    };

private:
    void increment(){
        if(_data != nullptr)
            ++(_data->count);
    }
    void decrement(){
        if(_data != nullptr)
            --(_data->count);
    }

    // If this is the only Pointer to the object, delete it
    // Otherwise decrement reference count
    void release(){
        if(_data != nullptr){
            if(unique()){
                // Delete the object
                delete _data->ptr;
                delete _data;
            } else {
                // Decrement reference count
                decrement();
            }
        }
    }

private:
    PointerData *_data;
};

}

#endif // ZPOINTER_H
