#ifndef ZNUMBER_H
#define ZNUMBER_H

#include "ztypes.h"
#include <cstring>
#include "zstring.h"

namespace LibChaos {

class ZNumber {
public:
    ZNumber() : _sign(true), _data(nullptr), _size(0){

    }

    ZNumber(void *value, zu64 bits){
        zu64 bytes = bits / 8;
        _data = new zbyte[bytes];
        memcpy(_data, value, bytes);
    }

    ZNumber(zs64 num) : ZNumber(){
        if(num){

        }
    }
    ZNumber(signed char num) : ZNumber((zs64)num){}
    ZNumber(signed short num) : ZNumber((zs64)num){}
    ZNumber(signed int num) : ZNumber((zs64)num){}
    ZNumber(signed long num) : ZNumber((zs64)num){}

    ZNumber(zu64 num){
        const zu64 bytes = 8;
        _data = new zbyte[bytes];
        memcpy(_data, &num, bytes);
    }
    ZNumber(unsigned char num) : ZNumber((zu64)num){}
    ZNumber(unsigned short num) : ZNumber((zu64)num){}
    ZNumber(unsigned int num) : ZNumber((zu64)num){}
    ZNumber(unsigned long num) : ZNumber((zu64)num){}

    ~ZNumber(){
        delete _data;
    }

    ZNumber factorial(){
        return ZNumber();
    }

    ZString str(){
        if(_data == nullptr){
            return "0";
        }
        return "!";
    }

    bool isPositive(){
        return _sign;
    }
    bool isNegative(){
        return !isPositive();
    }

    void *data() const {
        return _data;
    }
    zu64 size() const {
        return _size;
    }

private:
    bool _sign;
    zbyte *_data;
    zu64 _size;
};

}

#endif // ZNUMBER_H
