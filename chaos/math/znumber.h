/*******************************************************************************
**                                  LibChaos                                  **
**                                  znumber.h                                 **
**                          (c) 2015 Charlie Waters                           **
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
    typedef zu32 datatype;

public:
    //! Default constructor, initializes to positive zero.
    ZNumber();

//    ZNumber(void *value, zu64 bits) : ZNumber(){
//        zu64 bytes = bits / 8;
//        _data = new zbyte[bytes];
//        memcpy(_data, value, bytes);
//    }

    //! Initialize to @a num.
    ZNumber(zu64 num);
    //! Initialize to @a num.
    ZNumber(unsigned char num) : ZNumber((zu64)num){}
    //! Initialize to @a num.
    ZNumber(unsigned short num) : ZNumber((zu64)num){}
    //! Initialize to @a num.
    ZNumber(unsigned int num) : ZNumber((zu64)num){}
    //ZNumber(unsigned long num) : ZNumber((zu64)num){}

    //! Initialize to @a num.
    ZNumber(zs64 num);
    //! Initialize to @a num.
    ZNumber(signed char num) : ZNumber((zs64)num){}
    //! Initialize to @a num.
    ZNumber(char num) : ZNumber((zs64)num){}
    //ZNumber(signed short num) : ZNumber((zs64)num){}
    //! Initialize to @a num.
    ZNumber(short num) : ZNumber((zs64)num){}
    //ZNumber(signed int num) : ZNumber((zs64)num){}
    //! Initialize to @a num.
    ZNumber(int num) : ZNumber((zs64)num){}
    //ZNumber(signed long num) : ZNumber((zs64)num){}
    //ZNumber(long num) : ZNumber((zs64)num){}

    //! Initialize from @a str.
    ZNumber(ZString str);

    ZNumber(const ZNumber &other);

    ~ZNumber();

    // Conversions
    //operator zu64();

    // Assignment
    //! Assignment operator.
    ZNumber &operator=(const ZNumber &other);

    // Arithmetic Assignment
    //! Addition assignment operator.
    ZNumber &operator+=(const ZNumber &other);
    //! Subtraction assignment operator.
    ZNumber &operator-=(const ZNumber &other);
    //! Multiplication assignment operator.
    ZNumber &operator*=(const ZNumber &other);
    //! Division assignment operator.
    ZNumber &operator/=(const ZNumber &other);
    //! Modulus assignment operator.
    ZNumber &operator%=(const ZNumber &other);

    // Bitwise Assignment
    //! Bitwise AND assignment operator.
    ZNumber &operator&=(const ZNumber &other);
    //! Bitwise OR assignment operator.
    ZNumber &operator|=(const ZNumber &other);
    //! Bitwise XOR assignment operator.
    ZNumber &operator^=(const ZNumber &other);
    //! Bitwise Left Shift assignment operator.
    ZNumber &operator<<=(const ZNumber &other);
    //! Bitwise Right Shift assignment operator.
    ZNumber &operator>>=(const ZNumber &other);

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

    //! Prefix Increment.
    ZNumber &operator++();
    //! Postfix Increment.
    ZNumber operator++(int){
        ZNumber tmp(*this);
        operator++();
        return tmp;
    }

    //! Prefix Decrement.
    ZNumber &operator--();
    //! Postfix Decrement.
    ZNumber operator--(int){
        ZNumber tmp(*this);
        operator--();
        return tmp;
    }

    void clear();

    //! Calculate the factorial.
    ZNumber factorial() const;

    /*! Generate a numeric string with numeric @a base.
     *  @note Expensive!
     */
    ZString str(zu16 base = 10) const;

    ZString strBytes() const;

    // zero is positive (should it be negative too?)
    //! Check if number is positive.
    bool isPositive() const {
        return !isNegative();
    }
    //! Check if number is negative.
    bool isNegative() const;

    //! Pointer to raw data.
    datatype *data() const {
        return _data;
    }

    //! Size of data in bytes.
    zu64 size() const {
        return _size * sizeof(datatype);
    }

private:
    //! Pad end of data with @a num elements.
    void pad(zu64 num);
    //! Remove zeroed elements from end of data.
    void clean();

private:
    ZAllocator<datatype> *_alloc;
    bool _sign;
    datatype *_data;
    zu64 _size;
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
