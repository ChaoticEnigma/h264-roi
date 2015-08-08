/*******************************************************************************
**                                  LibChaos                                  **
**                                 znumber.cpp                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "znumber.h"

#ifndef ZNUMBER_REFERENCE

namespace LibChaos {

ZNumber::ZNumber() : _alloc(new ZAllocator<datatype>), _sign(false), _data(nullptr), _size(0){

}

ZNumber::ZNumber(zu64 num) : ZNumber(){
    _size = sizeof(zu64) / sizeof(datatype);
    _data = _alloc->alloc(_size);
    for(zu8 i = 0; i < _size; ++i){
        _data[i] = (datatype)(num >> ((sizeof(datatype) * 8) * i));
    }
}

ZNumber::ZNumber(zs64 num) : ZNumber(){
    _size = sizeof(zu64) / sizeof(datatype);
    _sign = num < 0;
    _data = _alloc->alloc(_size);
    for(zu8 i = 0; i < _size; ++i){
        _data[i] = (datatype)(num >> ((sizeof(datatype) * 8) * i));
    }
}

ZNumber::ZNumber(ZString str) : ZNumber(){

}

ZNumber::ZNumber(const ZNumber &other) : ZNumber(){
    if(other._size > 0){
        _size = other._size;
        _data = new datatype[_size];
        memcpy(_data, other._data, other._size);
        //_sign = other._sign;
    }
}

ZNumber::~ZNumber(){
    clear();
}

//ZNumber::operator zu64(){
//    return 0;
//}

ZNumber &ZNumber::operator=(const ZNumber &other){
    clear();
    if(other._size > 0){
        _size = other._size;
        _data = _alloc->alloc(_size);
        _alloc->copy(other._data, _data, _size);
        _sign = other._sign;
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
    datatype carry = 0;
    for(zu64 i = 0; i < MAX(_size, other._size); ++i){
        zu64 sum = _data[i] + other._data[i] + carry;
        _data[i] = (datatype)(sum);
        carry = (datatype)(sum >> (sizeof(datatype) * 8));
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
    _sign = true;
    _size = 0;
    _alloc->dealloc(_data);
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
    if(_data == nullptr || _size == 0){
        return "0";
    }
    if(base > 64){
        return "ERROR";
    }

    const char *digits =
        "01"       // Binary
        "234567"   // Octal
        "89"       // Decimal
        "abcdef"   // Hexadecimal
        "ghijklmnopqrstuv" // Base 32
        "wxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@"; // Base 64

    ZString out = (isNegative() ? "-" : "");
    bool bit = 0;
    for(zu64 i = 0; i < _size; ++i){
        for(zu8 j = 0; j < sizeof(datatype); ++j){
            datatype m = 1U << (j * 8);
            for(zu8 k = 0; k < 8; ++k){
                bit = _data[i] & m;
                m <<= 1;
                //out += (bit ? "1" : "0");
                out += digits[bit];
            }
        }
    }
    out.reverse();
    out.stripFront('0');
    return out;
}

ZString ZNumber::strBytes() const {
    if(_data == nullptr){
        return "null";
    }
    const char *digits = "01";
    ZString out;
    bool bit = 0;
    for(zu64 i = 0; i < _size; ++i){
        for(zu8 j = 0; j < sizeof(datatype); ++j){
            datatype m = 1U << (j * 8);
            for(zu8 k = 0; k < 8; ++k){
                bit = _data[i] & m;
                m <<= 1;
                out += digits[bit];
            }
            out += " ";
        }
    }
    out.stripBack(' ');
    out.reverse();
    return out;
}

bool ZNumber::isNegative() const {
    return _sign;
}

void ZNumber::pad(zu64 num){
    datatype *tmp = _data;
    _data = _alloc->alloc(_size + num);
    _alloc->copy(tmp, _data, _size);
    _alloc->dealloc(tmp);
    for(zu64 i = _size; i < _size + num; ++i){
        _data[i] = 0;
    }
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
    datatype *tmp = _data;
    _data = new datatype[length];
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
