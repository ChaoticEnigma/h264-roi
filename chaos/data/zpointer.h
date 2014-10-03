#ifndef ZPOINTER_H
#define ZPOINTER_H

#include "ztypes.h"
#include "zarray.h"

namespace LibChaos {

// Wraps a pointer to an arbitrary object, deletes it when the last Pointer to it is destroyed
// Note that all Pointers to the same object point to the *same object*, and can change the common object

// NEEDS EXTENSIVE TESTING

template <typename T> class ZPointer {
public:
    ZPointer() : _ptr(nullptr){

    }

    ZPointer(T *ptr) : _ptr(ptr){

    }

    // Copy other pointer and list
    // Create links for new object
    ZPointer(ZPointer &other) : _ptr(other._ptr), _others(other._others){
        if(_ptr != nullptr){
            addPointer(&other);
            // Tell other pointers to add this
            link();
        }
    }

    ZPointer &operator=(ZPointer &other){
        release();
        _ptr = other._ptr;
        _others = other._others;
        if(_ptr != nullptr){
            addPointer(&other);
            // Tell other pointers to add this
            link();
        }
        return *this;
    }

    ~ZPointer(){
        release();
    }

    // If this is the only Pointer to the object, delete it
    // Otherwise remove self from the Pointer lists of other Pointers to the same object
    void release(){
        if(unique()){
            // Delete the object
            delete _ptr;
        } else {
            // Tell other pointers to forget this
            unlink();
        }
    }

    void swap(ZPointer &other){
        // Make sure these are not related. That would be messy
        if(_ptr != other._ptr){
            // Make all linked Pointers forget these Pointers
            unlink();
            other.unlink();

            // Swap their contents
            T *ptr = _ptr;
            _ptr = other._ptr;
            other._ptr = ptr;
            other._others.swap(_others);

            // Make all linked Pointers remember these Pointers
            other.link();
            link();
        }
    }

    // Check if this is the only Pointer to the object
    bool unique() const {
        return (others() == 0);
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
    // Make sure every other Pointer knows about this Pointer
    void link(){
        for(zu64 i = 0; i < _others.size(); ++i){
            _others[i]->addPointer(this);
        }
    }

    // Make sure every other Pointer forgets about this Pointer
    void unlink(){
        for(zu64 i = 0; i < _others.size(); ++i)
            _others[i]->removePointer(this);
    }

    // Add a Pointer to the list if not a duplicate
    void addPointer(ZPointer *other){
        zu64 index = _others.indexOf(other);
        if(index == ZArray<ZPointer*>::none)
            _others.push(other);
    }

    // Remove a Pointer from the list
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
