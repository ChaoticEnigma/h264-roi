#ifndef ZNUMBER_H
#define ZNUMBER_H

#include "ztypes.h"
#include <cstring>
#include "zstring.h"
#include <bitset>

namespace LibChaos {

class ZNumber {
public:
    ZNumber() : _sign(true), _data(nullptr), _size(0){
        //clear(); // Redundant?
    }

//    ZNumber(void *value, zu64 bits) : ZNumber(){
//        zu64 bytes = bits / 8;
//        _data = new zbyte[bytes];
//        memcpy(_data, value, bytes);
//    }

    ZNumber(zs64 num) : ZNumber((num > 0) ? (zu64)num : (zu64)(num * -1)){
        _sign = (num > 0);
    }
    ZNumber(signed char num) : ZNumber((zs64)num){}
    ZNumber(char num) : ZNumber((zs64)num){}
    //ZNumber(signed short num) : ZNumber((zs64)num){}
    ZNumber(short num) : ZNumber((zs64)num){}
    //ZNumber(signed int num) : ZNumber((zs64)num){}
    ZNumber(int num) : ZNumber((zs64)num){}
    //ZNumber(signed long num) : ZNumber((zs64)num){}
    ZNumber(long num) : ZNumber((zs64)num){}

    ZNumber(zu64 num) : ZNumber(){
        zu8 length = 0;
        for(zu8 i = 0; i < sizeof(num); ++i){
            if(((zbyte*)&num)[i] == 0){
                length = i;
                break;
            }
        }
        if(!length)
            return;

        //        _size = length;
//        _data = new zbyte[_size];
//        zu8 j = length-1;
//        for(zu8 i = 0; i < length; ++i){
//            _data[i] = ((zbyte*)num)[j];
//            --j;
//        }

        _size = length;
        _data = new zbyte[_size];
        memcpy(_data, &num, _size);
    }
    ZNumber(unsigned char num) : ZNumber((zu64)num){}
    ZNumber(unsigned short num) : ZNumber((zu64)num){}
    ZNumber(unsigned int num) : ZNumber((zu64)num){}
    ZNumber(unsigned long num) : ZNumber((zu64)num){}

    ZNumber(const ZNumber &other) : ZNumber(){
        if(other._size > 0){
            _size = other._size;
            _data = new zbyte[_size];
            memcpy(_data, other._data, other._size);
            _sign = other._sign;
        }
    }

    ZNumber(ZString str) : ZNumber(){

    }

    ~ZNumber(){
        clear();
    }

    // Assignment
    ZNumber &operator=(const ZNumber &other){
        clear();
        if(other._size > 0){
            _size = other._size;
            _data = new zbyte[_size];
            memcpy(_data, other._data, other._size);
            _sign = other._sign;
        }
        return *this;
    }

    // Comparison
    friend bool operator==(const ZNumber &lhs, const ZNumber &rhs);
    friend bool operator!=(const ZNumber &lhs, const ZNumber &rhs);
    friend bool operator<(const ZNumber &lhs, const ZNumber &rhs);
    friend bool operator>(const ZNumber &lhs, const ZNumber &rhs);
    friend bool operator<=(const ZNumber &lhs, const ZNumber &rhs);
    friend bool operator>=(const ZNumber &lhs, const ZNumber &rhs);

    // Arithmetic
    friend ZNumber operator+(ZNumber lhs, const ZNumber &rhs);
    friend ZNumber operator-(ZNumber lhs, const ZNumber &rhs);
    friend ZNumber operator*(ZNumber lhs, const ZNumber &rhs);
    friend ZNumber operator/(ZNumber lhs, const ZNumber &rhs);

    // Arithmetic Assignment
    ZNumber &operator+=(const ZNumber &other){

        return *this;
    }
    ZNumber &operator-=(const ZNumber &other){

        return *this;
    }
    ZNumber &operator*=(const ZNumber &other){

        return *this;
    }
    ZNumber &operator/=(const ZNumber &other){

        return *this;
    }

    // Prefix Increment
    ZNumber &operator++(){

        return *this;
    }
    // Postfix Increment
    ZNumber operator++(int){
        ZNumber tmp(*this);
        operator++();
        return tmp;
    }

    // Prefix Decrement
    ZNumber &operator--(){

        return *this;
    }
    // Postfix Decrement
    ZNumber operator--(int){
        ZNumber tmp(*this);
        operator--();
        return tmp;
    }

    void clear(){
        _sign = true;
        _size = 0;
        delete[] _data;
        _data = nullptr;
    }

    ZNumber factorial() const {
        if(*this == 0){
            return ZNumber(1);
        }
        ZNumber result(*this);
        for(ZNumber next(*this-1); next > 1; --next){
            result *= next;
        }
        return result;
    }

    ZString str(zu16 base = 10) const {
        if(_data == nullptr){
            return "0";
        }

        ZString out;

        for(zu64 i = 0; i < _size; ++i){
            bool bit = 0;
            for(zu8 j = 1; j <= 128; j = j << 1){
                bit = _data[i] & j;
                out << (bit ? "1" : "0");
            }
            out << " ";
        }
        out.strip(' ');

        return out;
    }

    ZString strBytes() const {
        ZString out;
        for(zu64 i = 0; i < _size; ++i){
            std::bitset<8> b1(_data[i]);
            out = ZString(b1.to_string()) << " " << out;
        }
        out.strip(' ');
        return out;
    }

    bool isPositive() const {
        return _sign;
    }
    bool isNegative() const {
        return !isPositive();
    }

    zbyte *data() const {
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

// Comparison
inline bool operator==(const ZNumber &lhs, const ZNumber &rhs){
    return (lhs._sign == rhs._sign &&
            lhs._size == rhs._size &&
            memcmp(lhs._data, rhs._data, lhs._size) == 0);
}
inline bool operator!=(const ZNumber &lhs, const ZNumber &rhs){
    return !operator==(lhs,rhs);
}
inline bool operator<(const ZNumber &lhs, const ZNumber &rhs){
    if(!lhs._sign && rhs._sign)
        return true;
    if(lhs._sign && !rhs._sign)
        return false;
    if(lhs._size < rhs._size)
        return true;
    if(lhs._size > rhs._size)
        return false;

    return false;
}
inline bool operator>(const ZNumber &lhs, const ZNumber &rhs){
    return operator<(rhs,lhs);
}
inline bool operator<=(const ZNumber &lhs, const ZNumber &rhs){
    return !operator>(lhs,rhs);
}
inline bool operator>=(const ZNumber &lhs, const ZNumber &rhs){
    return !operator<(lhs,rhs);
}

// Arithmetic
inline ZNumber operator+(ZNumber lhs, const ZNumber &rhs){
    lhs += rhs;
    return lhs;
}
inline ZNumber operator-(ZNumber lhs, const ZNumber &rhs){
    lhs -= rhs;
    return lhs;
}
inline ZNumber operator*(ZNumber lhs, const ZNumber &rhs){
    lhs *= rhs;
    return lhs;
}
inline ZNumber operator/(ZNumber lhs, const ZNumber &rhs){
    lhs /= rhs;
    return lhs;
}

}

#endif // ZNUMBER_H
