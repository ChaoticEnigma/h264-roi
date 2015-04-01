#ifndef ZHASH
#define ZHASH

#include "ztypes.h"
#include <type_traits>

namespace LibChaos {

class ZHashBase {
public:
    enum hashMethod {
        integer,
        hashType1_64,
        xxHash64,
        defaultHash = hashType1_64,
    };
public:
    ZHashBase() : _size(0), _hash(nullptr){}
    virtual ~ZHashBase(){ delete[] _hash; }
    const zbyte *hash() const { return _hash; }
    const zbyte *operator()() const { return hash(); }
    zu8 hashSize() const { return _size; }

    static zu64 hashData(const zbyte *data, zu64 size, hashMethod method);

protected:
    ZHashBase(zu8 size, zbyte *hash) : _size(size), _hash(hash){}

    static void zu64toBytes(zu64 value, zbyte *hash){
        hash[0] = value >> 56 & 0xFF;
        hash[1] = value >> 48 & 0xFF;
        hash[2] = value >> 40 & 0xFF;
        hash[3] = value >> 32 & 0xFF;
        hash[4] = value >> 24 & 0xFF;
        hash[5] = value >> 16 & 0xFF;
        hash[6] = value >>  8 & 0xFF;
        hash[7] = value       & 0xFF;
    }
    static zu64 bytesToZu64(const zbyte *hash){
        return (zu64)hash[0] | (zu64)hash[1] <<  8 | (zu64)hash[2] << 16 | (zu64)hash[3] << 24 | (zu64)hash[4] << 32 | (zu64)hash[5] << 40 | (zu64)hash[6] << 48 | (zu64)hash[7] << 56;
    }

protected:
    zu8 _size;
    zbyte *_hash;
};

// Base ZHashMethod template
template <ZHashBase::hashMethod> class ZHashMethod;

template <> class ZHashMethod<ZHashBase::integer> : public ZHashBase {

};

template <> class ZHashMethod<ZHashBase::hashType1_64> : public ZHashBase {
public:
    //ZHashMethod(const zbyte *data, zu64 size){}
};

template <> class ZHashMethod<ZHashBase::xxHash64> : public ZHashBase {

};

// Base ZHash template
template <typename T, ZHashBase::hashMethod M = ZHashBase::defaultHash, typename = void> class ZHash;

template <typename T> class ZHash<T, ZHashBase::defaultHash, typename std::enable_if<std::is_enum<T>::value>::type> : public ZHashBase {
public:
    ZHash(T data) : ZHashBase((zu64)data){}
};

// z unsigned types
template <> class ZHash<zu64> : public ZHashBase {
public:
    ZHash(zu64 data) : ZHashBase(8, new zbyte[_size]){ zu64toBytes(data, _hash); }
    zu64 hashValue() const { return bytesToZu64(_hash); }
};
template <> class ZHash<zu8> : public ZHash<zu64> {
public:
    ZHash(zu8 data) : ZHash<zu64>((zu64)data){}
};
template <> class ZHash<zu16> : public ZHash<zu64> {
public:
    ZHash(zu16 data) : ZHash<zu64>((zu64)data){}
};
template <> class ZHash<zu32> : public ZHash<zu64> {
public:
    ZHash(zu32 data) : ZHash<zu64>((zu64)data){}
};

// z signed types
template <> class ZHash<zs8> : public ZHash<zu64> {
public:
    ZHash(zs8 data) : ZHash<zu64>((zu64)data){}
};
template <> class ZHash<zs16> : public ZHash<zu64> {
public:
    ZHash(zs16 data) : ZHash<zu64>((zu64)data){}
};
template <> class ZHash<zs32> : public ZHash<zu64> {
public:
    ZHash(zs32 data) : ZHash<zu64>((zu64)data){}
};
template <> class ZHash<zs64> : public ZHash<zu64> {
public:
    ZHash(zs64 data) : ZHash<zu64>((zu64)data){}
};

// signed
template <> class ZHash<char> : public ZHash<zu64> {
public:
    ZHash(char data) : ZHash<zu64>((zu64)data){}
};
template <> class ZHash<long> : public ZHash<zu64> {
public:
    ZHash(long data) : ZHash<zu64>((zu64)data){}
};

// unsigned
template <> class ZHash<unsigned long> : public ZHash<zu64> {
public:
    ZHash(unsigned long data) : ZHash<zu64>((zu64)data){}
};

}

#endif // ZHASH
