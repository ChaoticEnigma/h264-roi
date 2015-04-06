#ifndef ZHASH
#define ZHASH

#include "ztypes.h"
#include <type_traits>

namespace LibChaos {

class ZHashBase {
public:
    enum hashMethod {
        simpleHash64,
        xxHash64,
        defaultHash = xxHash64,
    };
public:
    virtual ~ZHashBase(){}

protected:
    static void zu64toBytes(zu64 value, zbyte *hash){
        hash[0] = value       & 0xFF;
        hash[1] = value >>  8 & 0xFF;
        hash[2] = value >> 16 & 0xFF;
        hash[3] = value >> 24 & 0xFF;
        hash[4] = value >> 32 & 0xFF;
        hash[5] = value >> 40 & 0xFF;
        hash[6] = value >> 48 & 0xFF;
        hash[7] = value >> 56 & 0xFF;
    }
    static zu64 bytesToZu64(const zbyte *hash){
        return (zu64)hash[0]       |
               (zu64)hash[1] <<  8 |
               (zu64)hash[2] << 16 |
               (zu64)hash[3] << 24 |
               (zu64)hash[4] << 32 |
               (zu64)hash[5] << 40 |
               (zu64)hash[6] << 48 |
               (zu64)hash[7] << 56;
    }
};

class ZHash64Base : public ZHashBase {
public:
    typedef zu64 hashtype;
public:
    virtual zu64 hash() const = 0;
public:
    static zu64 simpleHash64_hash(const zbyte *data, zu64 size);
    static zu64 xxHash64_hash(const zbyte *data, zu64 size);
};

// Base ZHashMethod template
template <ZHashBase::hashMethod> class ZHashMethod;

// Simple Hash (64-bit)
template <> class ZHashMethod<ZHashBase::simpleHash64> : public ZHash64Base {
public:
    ZHashMethod(const zbyte *data, zu64 size) : _hash(simpleHash64_hash(data, size)){}
    hashtype hash() const { return _hash; }
protected:
    hashtype _hash;
};

// XXH64 (64-bit)
template <> class ZHashMethod<ZHashBase::xxHash64> : public ZHash64Base {
public:
    ZHashMethod(const zbyte *data, zu64 size) : _hash(xxHash64_hash(data, size)){}
    hashtype hash() const { return _hash; }
protected:
    hashtype _hash;
};

// Base ZHash template <data type T, hash method M, SFINAE placeholder>
template <typename T, ZHashBase::hashMethod M = ZHashBase::defaultHash, typename = void> class ZHash;

// zu64 specialization
template <> class ZHash<zu64> : public ZHash64Base {
public:
    ZHash(zu64 data) : _hash(data){}
    hashtype hash() const { return _hash; }
private:
    hashtype _hash;
};

// enum specialization
template <typename T> class ZHash<T, ZHashBase::defaultHash, typename std::enable_if<std::is_enum<T>::value>::type> : public ZHash<zu64> {
public:
    ZHash(T data) : ZHash<zu64>((zu64)data){}
};

#define ZHASH_TRIVIAL_TEMPLATE(ARG)                 \
template <> class ZHash<ARG> : public ZHash<zu64> { \
public:                                             \
    ZHash(ARG data) : ZHash<zu64>((zu64)data){}     \
};

#define ZHASH_USER_SPECIALIAZATION(TYPE, ARGS, CONSTRUCT, IMPLEMENTATION) \
template <ZHashBase::hashMethod M> class ZHash<TYPE, M> : public ZHashMethod<M> { \
public: \
    ZHash ARGS : ZHashMethod<M> CONSTRUCT IMPLEMENTATION \
}; \
template <> class ZHash<TYPE, ZHashBase::defaultHash> : public ZHashMethod<ZHashBase::defaultHash> { \
public: \
    ZHash ARGS : ZHashMethod<ZHashBase::defaultHash> CONSTRUCT IMPLEMENTATION \
};

// integer specializations
ZHASH_TRIVIAL_TEMPLATE(bool)
ZHASH_TRIVIAL_TEMPLATE(char)
ZHASH_TRIVIAL_TEMPLATE(zu8)
ZHASH_TRIVIAL_TEMPLATE(zs8)

ZHASH_TRIVIAL_TEMPLATE(zu16)
ZHASH_TRIVIAL_TEMPLATE(zs16)

ZHASH_TRIVIAL_TEMPLATE(zu32)
ZHASH_TRIVIAL_TEMPLATE(zs32)

//ZHASH_TRIVIAL_TEMPLATE(long)
//ZHASH_TRIVIAL_TEMPLATE(unsigned long)

ZHASH_TRIVIAL_TEMPLATE(zs64)

}

#endif // ZHASH
