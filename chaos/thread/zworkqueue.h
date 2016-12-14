#ifndef ZWORKQUEUE_H
#define ZWORKQUEUE_H

#include "zqueue.h"
#include "zmutex.h"
#include "zcondition.h"
#include "zlog.h"

namespace LibChaos {

/*! Thread-safe work distribution utility.
 *  \ingroup Thread
 */
template <class T> class ZWorkQueue {
public:
    ZWorkQueue(){}

    void addWork(const T &item){
        _condtion.lock();

        _queue.push(item);

//        LOG("signal");
        _condtion.signal();
        _condtion.unlock();
    }

    T getWork(){
        _condtion.lock();
        while(_queue.size() == 0){
//            LOG("block");
            _condtion.wait();
        }

        T item = _queue.peek();
        _queue.pop();

//        LOG("done");
        _condtion.unlock();
        return item;
    }

    zu64 size(){
        _condtion.lock();
        zu64 sz = _queue.size();
        _condtion.unlock();
        return sz;
    }

private:
    ZQueue<T> _queue;
    ZCondition _condtion;
};

} // namespace LibChaos

#endif // ZWORKQUEUE_H
