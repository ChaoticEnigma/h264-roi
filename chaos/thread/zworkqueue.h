#ifndef ZWORKQUEUE_H
#define ZWORKQUEUE_H

#include "ypushpopaccess.h"
#include "zqueue.h"
#include "zmutex.h"
#include "zcondition.h"

namespace LibChaos {

/*! Thread-safe work distribution utility.
 *  \ingroup Thread
 */
template <class T> class ZWorkQueue {
public:
    ZWorkQueue(){}

    void addWork(const T &item){
        _condtion.waitLock();
        _queue.push(item);
        _condtion.signal();
        _condtion.waitUnlock();
    }

    T getWork(){
        _condtion.waitLock();
        while(_queue.size() == 0)
            _condtion.wait();
        T item = _queue.peek();
        _queue.pop();
        _condtion.waitUnlock();
        return item;
    }

    zu64 size(){
        _condtion.waitLock();
        zu64 sz = _queue.size();
        _condtion.waitUnlock();
        return sz;
    }

private:
    ZQueue<T> _queue;
    ZCondition _condtion;
};

} // namespace LibChaos

#endif // ZWORKQUEUE_H
