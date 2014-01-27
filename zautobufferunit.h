#ifndef ZAUTOBUFFERUNIT_H
#define ZAUTOBUFFERUNIT_H

#include "zstring.h"

namespace LibChaos {

class BufferUnit {
public:
    zu64 size(){
        return _size;
    }
    void forceSize(zu64 sz){
        _size = sz;
    }
private:
    zu64 _size;
};



}

#endif // ZAUTOBUFFERUNIT_H
