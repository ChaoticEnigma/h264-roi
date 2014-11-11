#ifndef ZQUEUE_H
#define ZQUEUE_H

#include "ztypes.h"
#include "zlist.h"

#include "ypushpopaccess.h"

namespace LibChaos {

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

private:
    ZList<T> _data;
};

}

#endif // ZQUEUE_H
