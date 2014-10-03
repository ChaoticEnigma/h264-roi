#ifndef ZPOINTER_H
#define ZPOINTER_H

#include "ztypes.h"
#include "zarray.h"

namespace LibChaos {

// Wraps a pointer to an arbitrary object, deletes it when the last Pointer to it is destroyed
// Note that all Pointers to the same object point to the *same object*, and can change the common object

template <typename T> class ZPointer {
public:
    ZPointer() : _ptr(nullptr){

    }

    ZPointer(T *ptr) : _ptr(ptr){

    }

    // If this is the only Pointer to the object, delete it
    // Otherwise remove self from the Pointer lists of other Pointers to the same object
    ~ZPointer(){
        if(unique())
            delete _ptr;
        else
            unlink();
    }

    // Add other to Pointer list
    // Add self to Pointer list of other
    ZPointer(ZPointer &other){
        link(&other);
    }

    void swap(ZPointer &other){
        T *ptr = _ptr;
        _ptr = other._ptr;
        other._ptr = ptr;
        other._others.swap(_others);
    }

    // Check if this is the only Pointer to the object
    bool unique() const {
        return !others();
    }

    // Count number of other Pointers to the object
    zu64 others() const {
        return _others.size();
    }

    // Get pointer
    T *ptr() const {
        return _ptr;
    }

    // Get reference
    T &get(){
        return *_ptr;
    }
    const T&get() const {
        return *_ptr;
    }

private:
    void link(ZPointer *other){
        addPointer(other);
        other->addPointer(this);
    }
    void unlink(){
        for(zu64 i = 0; i < _others.size(); ++i)
            _others[i]->removePointer(this);
        _others.clear();
    }

    void addPointer(ZPointer *other){
        _others.push(other);
    }
    void removePointer(ZPointer *other){
        zu64 index = _others.indexOf(other);
        if(index < _others.size())
            _others.erase(index);
    }

private:
    T *_ptr;
    ZArray<ZPointer*> _others;
};

}

#endif // ZPOINTER_H
