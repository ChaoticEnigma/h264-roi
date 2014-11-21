#include "znumber.h"

#ifdef ZNUMBER_REFERENCE

namespace LibChaos {

ZNumber::ZNumber() : _sign(true), _number(0){

}

ZNumber::ZNumber(zs64 num) : ZNumber(){
    if(num < 0){
        _sign = false;
        _number = (zu64)-num;
    } else {
        _number = (zu64)num;
    }
}

ZNumber::ZNumber(zu64 num) : ZNumber(){
    _number = num;
}

ZNumber::ZNumber(const ZNumber &other) : ZNumber(){
    _sign = other._sign;
    _number = other._number;
}

ZNumber::ZNumber(ZString str) : ZNumber(){

}

ZNumber &ZNumber::operator=(const ZNumber &other){
    _sign = other._sign;
    _number = other._number;
    return *this;
}

ZNumber ZNumber::operator-(){
    ZNumber tmp = *this;
    tmp._sign = !tmp._sign;
    return tmp;
}

ZNumber &ZNumber::operator+=(const ZNumber &other){
    if(_sign){
        if(other._sign){
            _number += other._number;
        } else {
            if(other._number > _number){

            } else {
                _number -= other._number;
            }
        }
    } else {
        if(other._sign){
            _number -= other._number;
        } else {
            _number += other._number;
        }
    }
    return *this;
}

ZNumber &ZNumber::operator-=(const ZNumber &other){
    operator+=(-other);
    return *this;
}

ZNumber &ZNumber::operator*=(const ZNumber &other){
    if((!_sign && other._sign) || (_sign && !other._sign))
        _sign = false;
    else
        _sign = true;
    _number *= other._number;
    return *this;
}

ZNumber &ZNumber::operator/=(const ZNumber &other){
    if(other == 0){
        throw ZError("Division by zero");
    }
    return *this;
}

ZNumber &ZNumber::operator%=(const ZNumber &other){
    if(other == 0){
        throw ZError("Modulus by zero");
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
    for(ZNumber next(*this-1); next > 1; --next){
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
