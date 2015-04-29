/*******************************************************************************
**                                  LibChaos                                  **
**                                  zhash.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZHASH
#define ZHASH

#include "ztypes.h"
#include <type_traits>

// Simple Hash initial base
#define ZHASH_SIMPLEHASH_INIT ((zu64)5381)

// FNV-1(a) initial base is the the FNV-0 hash of "chongo <Landon Curt Noll> /\../\" (32 bytes)
#define ZHASH_FNV1A_64_INIT ((zu64)0xcbf29ce484222325ULL)

#define ZHASH_CRC32_INIT ((zu32)0x0)

namespace LibChaos {

class ZHashBase {
public:
    enum hashMethod {
        simpleHash64,
        xxHash64,
        fnvHash64,
        crc32,
        defaultHash = fnvHash64,
    };
public:
    virtual ~ZHashBase(){}
};

class ZHash32Base : public ZHashBase {
public:
    typedef zu32 hashtype;

public:
    ZHash32Base(zu32 hash) : _hash(hash){}
    virtual zu32 hash() const { return _hash; }

public:
    static zu32 crcHash32_hash(const zbyte *data, zu64 size, zu32 remainder = ZHASH_CRC32_INIT);

protected:
    hashtype _hash;
};

class ZHash64Base : public ZHashBase {
public:
    typedef zu64 hashtype;

public:
    ZHash64Base(zu64 hash) : _hash(hash){}
    virtual zu64 hash() const { return _hash; }

public:
    static zu64 simpleHash64_hash(const zbyte *data, zu64 size, zu64 seed = ZHASH_SIMPLEHASH_INIT);

    static zu64 fnvHash64_hash(const zbyte *data, zu64 size, zu64 hval = ZHASH_FNV1A_64_INIT);

    static zu64 xxHash64_hash(const zbyte *data, zu64 size);
    static void *xxHash64_init();
    static void xxHash64_feed(void *state, const zbyte *data, zu64 size);
    static zu64 xxHash64_done(void *state);

protected:
    virtual void feedHash(const zbyte *data, zu64 size) = 0;
    virtual void doneHash(){}

protected:
    hashtype _hash;
};

// Base ZHashMethod template
template <ZHashBase::hashMethod> class ZHashMethod;

// CRC-32 (32-bit)
template <> class ZHashMethod<ZHashBase::crc32> : public ZHash32Base {
public:
    ZHashMethod() : ZHash32Base(ZHASH_CRC32_INIT){}
    ZHashMethod(const zbyte *data, zu64 size) : ZHash32Base(crcHash32_hash(data, size)){}
protected:
    void feedHash(const zbyte *data, zu64 size){
        _hash = crcHash32_hash(data, size, _hash);
    }
};

// Simple Hash (64-bit)
template <> class ZHashMethod<ZHashBase::simpleHash64> : public ZHash64Base {
public:
    ZHashMethod() : ZHash64Base(ZHASH_SIMPLEHASH_INIT){}
    ZHashMethod(const zbyte *data, zu64 size) : ZHash64Base(simpleHash64_hash(data, size)){}
protected:
    void feedHash(const zbyte *data, zu64 size){
        _hash = simpleHash64_hash(data, size, _hash);
    }
};

// FNV-1a Hash (64-bit)
template <> class ZHashMethod<ZHashBase::fnvHash64> : public ZHash64Base {
public:
    ZHashMethod() : ZHash64Base(ZHASH_FNV1A_64_INIT){}
    ZHashMethod(const zbyte *data, zu64 size) : ZHash64Base(fnvHash64_hash(data, size)){}
protected:
    void feedHash(const zbyte *data, zu64 size){
        _hash = fnvHash64_hash(data, size, _hash);
    }
};

// XXH64 (64-bit)
template <> class ZHashMethod<ZHashBase::xxHash64> : public ZHash64Base {
public:
    ZHashMethod() : ZHash64Base(0), _state(xxHash64_init()){}
    ZHashMethod(const zbyte *data, zu64 size) : ZHash64Base(xxHash64_hash(data, size)), _state(nullptr){}
protected:
    void feedHash(const zbyte *data, zu64 size){
        if(_state != nullptr)
            xxHash64_feed(_state, data, size);
    }
    void doneHash(){
        if(_state != nullptr)
            _hash = xxHash64_done(_state);
        _state = nullptr;
    }
protected:
    void *_state;
};

// Base ZHash template <data type T, hash method M, SFINAE placeholder>
template <typename T, ZHashBase::hashMethod M = ZHashBase::defaultHash, typename = void> class ZHash;

// zu64 specialization
template <> class ZHash<zu64> : public ZHash64Base {
public:
    ZHash(zu64 data) : ZHash64Base(data){}
protected:
    void feedHash(const zbyte *data, zu64 size){}
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

// integer specializations
ZHASH_TRIVIAL_TEMPLATE(bool)
ZHASH_TRIVIAL_TEMPLATE(char)
ZHASH_TRIVIAL_TEMPLATE(zu8)
ZHASH_TRIVIAL_TEMPLATE(zs8)

ZHASH_TRIVIAL_TEMPLATE(zu16)
ZHASH_TRIVIAL_TEMPLATE(zs16)

ZHASH_TRIVIAL_TEMPLATE(zu32)
ZHASH_TRIVIAL_TEMPLATE(zs32)

ZHASH_TRIVIAL_TEMPLATE(zs64)

#if COMPILER == MINGW
  ZHASH_TRIVIAL_TEMPLATE(long)
  ZHASH_TRIVIAL_TEMPLATE(unsigned long)
#endif

#define ZHASH_USER_SPECIALIAZATION(TYPE, ARGS, CONSTRUCT, IMPLEMENTATION) \
template <ZHashBase::hashMethod M> class ZHash<TYPE, M> : public ZHashMethod<M> { \
public: \
    ZHash ARGS : ZHashMethod<M> CONSTRUCT IMPLEMENTATION \
}; \
template <> class ZHash<TYPE, ZHashBase::defaultHash> : public ZHashMethod<ZHashBase::defaultHash> { \
public: \
    ZHash ARGS : ZHashMethod<ZHashBase::defaultHash> CONSTRUCT IMPLEMENTATION \
};

}

#endif // ZHASH
