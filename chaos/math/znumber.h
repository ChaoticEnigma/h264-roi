#ifndef ZNUMBER_H
#define ZNUMBER_H

#include "ztypes.h"
#include <cstring>
#include "zstring.h"
#include <bitset>
#include "zerror.h"

namespace LibChaos {

// NOTES
// Using twos-complement
// -a = ~a + 1

class ZNumber {
public:
    ZNumber() : _data(nullptr), _size(0){
        //clear(); // Redundant?
    }

//    ZNumber(void *value, zu64 bits) : ZNumber(){
//        zu64 bytes = bits / 8;
//        _data = new zbyte[bytes];
//        memcpy(_data, value, bytes);
//    }

    ZNumber(zs64 num) : ZNumber(){
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
            //_sign = other._sign;
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
            //_sign = other._sign;
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
    friend ZNumber operator%(ZNumber lhs, const ZNumber &rhs);

    // Unary
    friend ZNumber operator+(ZNumber a);
    friend ZNumber operator-(ZNumber a);

    // Bitwise
    friend ZNumber operator~(ZNumber a);
    friend ZNumber operator&(ZNumber lhs, const ZNumber &rhs);
    friend ZNumber operator|(ZNumber lhs, const ZNumber &rhs);
    friend ZNumber operator^(ZNumber lhs, const ZNumber &rhs);
    friend ZNumber operator<<(ZNumber lhs, const ZNumber &rhs);
    friend ZNumber operator>>(ZNumber lhs, const ZNumber &rhs);

    // Arithmetic Assignment
    ZNumber &operator+=(const ZNumber &other){
        if(other.isNegative()){
            throw ZError("wait");
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
    ZNumber &operator-=(const ZNumber &other){

        return *this;
    }
    ZNumber &operator*=(const ZNumber &other){

        return *this;
    }
    ZNumber &operator/=(const ZNumber &other){
        if(other == 0){
            throw ZError("Division by zero");
        }
        return *this;
    }
    ZNumber &operator%=(const ZNumber &other){
        if(other == 0){
            throw ZError("Division by zero");
        }
        return *this;
    }

    // Bitwise Assignment
    ZNumber &operator&=(const ZNumber &other){

        return *this;
    }
    ZNumber &operator|=(const ZNumber &other){

        return *this;
    }
    ZNumber &operator^=(const ZNumber &other){

        return *this;
    }
    ZNumber &operator<<=(const ZNumber &other){

        return *this;
    }
    ZNumber &operator>>=(const ZNumber &other){

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
        //_sign = true;
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

    // Still working this out...
    ZString str(zu16 base = 10) const {
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

    ZString strBytes() const {
        ZString out;
        for(zu64 i = 0; i < _size; ++i){
            std::bitset<8> b1(_data[i]);
            out = ZString(b1.to_string()) << " " << out;
        }
        out.strip(' ');
        return out;
    }

    // zero is positive (should it be negative too?)
    bool isPositive() const {
        return !isNegative();
    }
    bool isNegative() const {
        return _data[_size-1] & 128;
    }

    zbyte *data() const {
        return _data;
    }
    zu64 size() const {
        return _size;
    }

    void pad(zu64 num){
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

    void clean(){
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

    zbyte byte(zu64 i) const {
        return (i < _size ? _data[i] : 0);
    }

private:
    zbyte *_data;
    zu64 _size;
};

// Comparison
inline bool operator==(const ZNumber &lhs, const ZNumber &rhs){
    return (lhs._size == rhs._size &&
            memcmp(lhs._data, rhs._data, lhs._size) == 0);
}
inline bool operator!=(const ZNumber &lhs, const ZNumber &rhs){
    return !operator==(lhs,rhs);
}
inline bool operator<(const ZNumber &lhs, const ZNumber &rhs){
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
inline ZNumber operator%(ZNumber lhs, const ZNumber &rhs){
    lhs %= rhs;
    return lhs;
}

//Unary
inline ZNumber operator+(ZNumber a){
    return a;
}
inline ZNumber operator-(ZNumber a){
    return a;
}

// Bitwise
inline ZNumber operator~(ZNumber a){

    return a;
}
inline ZNumber operator&(ZNumber lhs, const ZNumber &rhs){
    lhs &= rhs;
    return lhs;
}
inline ZNumber operator|(ZNumber lhs, const ZNumber &rhs){
    lhs |= rhs;
    return lhs;
}
inline ZNumber operator^(ZNumber lhs, const ZNumber &rhs){
    lhs ^= rhs;
    return lhs;
}
inline ZNumber operator<<(ZNumber lhs, const ZNumber &rhs){
    lhs <<= rhs;
    return lhs;
}
inline ZNumber operator>>(ZNumber lhs, const ZNumber &rhs){
    lhs >>= rhs;
    return lhs;
}

}

#endif // ZNUMBER_H
