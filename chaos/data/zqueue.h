/*******************************************************************************
**                                  LibChaos                                  **
**                                  zqueue.h                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZQUEUE_H
#define ZQUEUE_H

#include "ztypes.h"
#include "zlist.h"

#include "ypushpopaccess.h"

namespace LibChaos {

//! FIFO queue ZList wrapper.
template <typename T> class ZQueue : public YPushPopAccess<T> {
public:
    ZQueue() : _data(){}

    void push(const T &data){
        _data.pushBack(data);
    }

    T &peek(){
        return _data.front();
    }
    const T &peek() const {
        return _data.front();
    }

    void pop(){
        _data.popFront();
    }

    void swap(ZQueue &other){
        _data.swap(other._data);
    }

    bool isEmpty() const {
        return _data.isEmpty();
    }

    zu64 size() const {
        return _data.size();
    }

private:
    ZList<T> _data;
};

}

#endif // ZQUEUE_H
