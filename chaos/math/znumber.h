/*******************************************************************************
**                                  LibChaos                                  **
**                                  znumber.h                                 **
**                          (c) 2015 Zennix Studios                           **
*******************************************************************************/
#ifndef ZNUMBER_H
#define ZNUMBER_H

#include "ztypes.h"
#include <cstring>
#include "zstring.h"
#include <bitset>
#include "zexception.h"

// Use reference implementation
//#define ZNUMBER_REFERENCE

namespace LibChaos {

// TODO: Operators to implement
// ZNumber(zs64)
// ZNumber(zu64)
// ZNumber(ZString)

// operator+=
// operator-=
// operator*=
// operator/=
// operator%=

// operator&=
// operator|=
// operator^=
// operator<<=
// operator>>=

// str

// operator<

// operator+
// operator-
// operator~


// ZNumber: Represents a floating point or integral number of variable size
// Numbers stored in little endian
// Using twos-complement
// -a = ~a + 1
class ZNumber {
public:
    ZNumber();

//    ZNumber(void *value, zu64 bits) : ZNumber(){
//        zu64 bytes = bits / 8;
//        _data = new zbyte[bytes];
//        memcpy(_data, value, bytes);
//    }

    ZNumber(zs64 num);
    ZNumber(signed char num) : ZNumber((zs64)num){}
    ZNumber(char num) : ZNumber((zs64)num){}
    //ZNumber(signed short num) : ZNumber((zs64)num){}
    ZNumber(short num) : ZNumber((zs64)num){}
    //ZNumber(signed int num) : ZNumber((zs64)num){}
    ZNumber(int num) : ZNumber((zs64)num){}
    //ZNumber(signed long num) : ZNumber((zs64)num){}
    //ZNumber(long num) : ZNumber((zs64)num){}

    ZNumber(zu64 num);
    ZNumber(unsigned char num) : ZNumber((zu64)num){}
    ZNumber(unsigned short num) : ZNumber((zu64)num){}
    ZNumber(unsigned int num) : ZNumber((zu64)num){}
    //ZNumber(unsigned long num) : ZNumber((zu64)num){}

    ZNumber(const ZNumber &other);

    ZNumber(ZString str);

    ~ZNumber(){ clear(); }

    // Conversions
    //operator zu64();

    // Assignment
    ZNumber &operator=(const ZNumber &other);

    // Comparison
    friend bool operator==(const ZNumber &lhs, const ZNumber &rhs);
    friend bool operator!=(const ZNumber &lhs, const ZNumber &rhs); // Uses ==
    friend bool operator<(const ZNumber &lhs, const ZNumber &rhs);
    friend bool operator>(const ZNumber &lhs, const ZNumber &rhs); // Uses == and <
    friend bool operator<=(const ZNumber &lhs, const ZNumber &rhs); // Uses >
    friend bool operator>=(const ZNumber &lhs, const ZNumber &rhs); // Uses <

    // Arithmetic
    friend ZNumber operator+(ZNumber lhs, const ZNumber &rhs); // Uses +=
    friend ZNumber operator-(ZNumber lhs, const ZNumber &rhs); // Uses -=
    friend ZNumber operator*(ZNumber lhs, const ZNumber &rhs); // Uses *=
    friend ZNumber operator/(ZNumber lhs, const ZNumber &rhs); // Uses /=
    friend ZNumber operator%(ZNumber lhs, const ZNumber &rhs); // Uses %=

    // Unary
    //friend ZNumber operator+(ZNumber a);
    ZNumber operator-() const;

    // Bitwise
    friend ZNumber operator~(ZNumber a);
    friend ZNumber operator&(ZNumber lhs, const ZNumber &rhs); // Uses &=
    friend ZNumber operator|(ZNumber lhs, const ZNumber &rhs); // Uses |=
    friend ZNumber operator^(ZNumber lhs, const ZNumber &rhs); // Uses ^=
    friend ZNumber operator<<(ZNumber lhs, const ZNumber &rhs); // Uses <<=
    friend ZNumber operator>>(ZNumber lhs, const ZNumber &rhs); // Uses >>=

    // Arithmetic Assignment
    ZNumber &operator+=(const ZNumber &other);
    ZNumber &operator-=(const ZNumber &other);
    ZNumber &operator*=(const ZNumber &other);
    ZNumber &operator/=(const ZNumber &other);
    ZNumber &operator%=(const ZNumber &other);

    // Bitwise Assignment
    ZNumber &operator&=(const ZNumber &other);
    ZNumber &operator|=(const ZNumber &other);
    ZNumber &operator^=(const ZNumber &other);
    ZNumber &operator<<=(const ZNumber &other);
    ZNumber &operator>>=(const ZNumber &other);

    // Prefix Increment
    ZNumber &operator++();
    // Postfix Increment
    ZNumber operator++(int){
        ZNumber tmp(*this);
        operator++();
        return tmp;
    }

    // Prefix Decrement
    ZNumber &operator--();
    // Postfix Decrement
    ZNumber operator--(int){
        ZNumber tmp(*this);
        operator--();
        return tmp;
    }

    void clear();

    ZNumber factorial() const;

    // Expensive!
    ZString str(zu16 base = 10) const;

    ZString strBytes() const;

    // zero is positive (should it be negative too?)
    bool isPositive() const {
        return !isNegative();
    }
    bool isNegative() const;

#ifdef ZNUMBER_REFERENCE
    zbyte *data() const {
        return (zbyte *)&_number;
    }
    zu64 size() const {
        return 8;
    }
#else
    zbyte *data() const {
        return _data;
    }
    zu64 size() const {
        return _size;
    }
#endif

    zbyte byte(zu64 i) const {
        return (i < size() ? data()[i] : 0);
    }

private:
    void pad(zu64 num);

    void clean();

private:
#ifdef ZNUMBER_REFERENCE
    bool _sign;
    zu64 _number;
#else
    zbyte *_data;
    zu64 _size;
#endif
};

// Comparison
bool operator==(const ZNumber &lhs, const ZNumber &rhs);
inline bool operator!=(const ZNumber &lhs, const ZNumber &rhs){
    return !operator==(lhs, rhs);
}
bool operator<(const ZNumber &lhs, const ZNumber &rhs);
inline bool operator>(const ZNumber &lhs, const ZNumber &rhs){
    return !operator==(lhs, rhs) && !operator<(rhs, lhs);
}
inline bool operator<=(const ZNumber &lhs, const ZNumber &rhs){
    return !operator>(lhs, rhs);
}
inline bool operator>=(const ZNumber &lhs, const ZNumber &rhs){
    return !operator<(lhs, rhs);
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
//inline ZNumber operator+(ZNumber a){
//    return a;
//}

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
