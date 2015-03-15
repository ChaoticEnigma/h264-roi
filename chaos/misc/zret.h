#ifndef ZRET_H
#define ZRET_H

#include "ztypes.h"
#include "zerror.h"

namespace LibChaos {

template <typename T> class ZRet : public T {
public:
    ZRet(){}
    ZRet(T val) : T(val), code(true){}
    ZRet(bool stat) : code(stat){}

    ZRet(ZException err) : error(err){}

    operator bool(){
        return code;
    }
//    operator T(){
//        return value;
//    }
private:
//    T value;
    bool code;
    ZException error;
};

}

#endif // ZRET_H
