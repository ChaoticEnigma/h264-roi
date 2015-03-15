#include "znumber.h"

#ifndef ZNUMBER_REFERENCE

namespace LibChaos {

ZNumber::ZNumber() : _data(nullptr), _size(0){
    //clear(); // Redundant?
}

ZNumber::ZNumber(zs64 num) : ZNumber(){
    zu8 length = 0;
    for(zu8 i = 0; i < sizeof(num); ++i){
        if(((zbyte*)&num)[i] == 0){
            length = i;
            break;
        }
    }
    if(!length)
        return;
    _size = length;
    _data = new zbyte[_size];
    memcpy(_data, &num, _size);
}

ZNumber::ZNumber(zu64 num) : ZNumber(){
    zu8 length = 0;
    for(zu8 i = 0; i < sizeof(num); ++i){
        if(((zbyte*)&num)[i] == 0){
            length = i;
            break;
        }
    }
    if(!length)
        return;
    _size = length;
    _data = new zbyte[_size];
    memcpy(_data, &num, _size);
}

ZNumber::ZNumber(const ZNumber &other) : ZNumber(){
    if(other._size > 0){
        _size = other._size;
        _data = new zbyte[_size];
        memcpy(_data, other._data, other._size);
        //_sign = other._sign;
    }
}

ZNumber::ZNumber(ZString str) : ZNumber(){

}

//ZNumber::operator zu64(){
//    return 0;
//}

ZNumber &ZNumber::operator=(const ZNumber &other){
    clear();
    if(other._size > 0){
        _size = other._size;
        _data = new zbyte[_size];
        memcpy(_data, other._data, other._size);
        //_sign = other._sign;
    }
    return *this;
}

ZNumber ZNumber::operator-() const {

    return *this;
}

ZNumber &ZNumber::operator+=(const ZNumber &other){
    if(other.isNegative()){
        throw ZException("wait");
        return *this;
    }
    if(other._size > _size){
        pad(other._size - _size);
    }
    zu8 carry = 0;
    for(zu64 i = 0; i < MAX(_size, other._size); ++i){
        zu16 sum = byte(i) + other.byte(i) + carry;
        _data[i] = ((zbyte*)&sum)[0];
        carry = ((zbyte*)&sum)[1];
    }
    if(carry){
        pad(1);
        _data[_size - 1] = carry;
    }
    return *this;
}

ZNumber &ZNumber::operator-=(const ZNumber &other){

    return *this;
}

ZNumber &ZNumber::operator*=(const ZNumber &other){

    return *this;
}

ZNumber &ZNumber::operator/=(const ZNumber &other){
    if(other == 0){
        throw ZException("Division by zero");
    }
    return *this;
}

ZNumber &ZNumber::operator%=(const ZNumber &other){
    if(other == 0){
        throw ZException("Modulus by zero");
    }
    return *this;
}

ZNumber &ZNumber::operator&=(const ZNumber &other){

    return *this;
}

ZNumber &ZNumber::operator|=(const ZNumber &other){

    return *this;
}

ZNumber &ZNumber::operator^=(const ZNumber &other){

    return *this;
}

ZNumber &ZNumber::operator<<=(const ZNumber &other){

    return *this;
}

ZNumber &ZNumber::operator>>=(const ZNumber &other){

    return *this;
}

ZNumber &ZNumber::operator++(){

    return *this;
}

ZNumber &ZNumber::operator--(){

    return *this;
}

void ZNumber::clear(){
    //_sign = true;
    _size = 0;
    delete[] _data;
    _data = nullptr;
}

ZNumber ZNumber::factorial() const {
    if(*this == 0){
        return ZNumber(1);
    }
    ZNumber result(*this);
    for(ZNumber next(*this - 1); next > 1; --next){
        result *= next;
    }
    return result;
}

// Still working this out...
ZString ZNumber::str(zu16 base) const {
    if(_data == nullptr){
        return "0";
    }
    if(base > 64){
        return "!";
    }

    const char *digits =
        "01"       // Binary
        "234567"   // Octal
        "89"       // Decimal
        "abcdef"   // Hexadecimal
        "ghijklmnopqrstuv" // Base 32
        "wxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@"; // Base 64

    ZString out;

    bool bit = 0;
    for(zu64 i = 0; i < _size; ++i){
        for(zu8 j = 0, m = 1; j < 8; ++j, m <<= 1){
            bit = _data[i] & m;

            //out += (bit ? "1" : "0");
            out += digits[bit];
        }
        out += " ";
    }
    out.strip(' ');

    return out;
}

ZString ZNumber::strBytes() const {
    ZString out;
    for(zu64 i = 0; i < _size; ++i){
        std::bitset<8> b1(_data[i]);
        out = ZString(b1.to_string()) << " " << out;
    }
    out.strip(' ');
    return out;
}

bool ZNumber::isNegative() const {
    return _data[_size-1] & 128;
}

void ZNumber::pad(zu64 num){
    zbyte *tmp = _data;
    _data = new zbyte[_size + num];
    memcpy(_data, tmp, _size);
    delete[] tmp;
    for(zu64 i = _size; i < _size + num; ++i){
        _data[i] = 0;
    }
    //memset(_data + _size, 0, num);
    _size += num;
}

void ZNumber::clean(){
    zu8 length = 0;
    for(zu8 i = 0; i < _size; ++i){
        if(_data[i] == 0){
            length = i;
            break;
        }
    }
    zbyte *tmp = _data;
    _data = new zbyte[length];
    memcpy(_data, tmp, length);
    delete[] tmp;
    _size = length;
}

bool operator==(const ZNumber &lhs, const ZNumber &rhs){
    return (lhs._size == rhs._size && (memcmp(lhs._data, rhs._data, lhs._size) == 0));
}

bool operator<(const ZNumber &lhs, const ZNumber &rhs){
    if(lhs._size < rhs._size)
        return true;
    if(lhs._size > rhs._size)
        return false;

    return false;
}

}

#endif
