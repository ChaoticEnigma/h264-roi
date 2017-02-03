/*******************************************************************************
**                                  LibChaos                                  **
**                                  zhash.h                                   **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZHASH
#define ZHASH

#include "ztypes.h"
#include "zbinary.h"
#include "zstring.h"

#include <type_traits>

// Simple Hash initial base
#define ZHASH_SIMPLEHASH_INIT ((zu64)5381)

// FNV-1(a) initial base is the the FNV-0 hash of "chongo <Landon Curt Noll> /\../\" (32 bytes)
#define ZHASH_FNV1A_64_INIT ((zu64)0xcbf29ce484222325ULL)

// CRC 32 initial remainder
#define ZHASH_CRC32_INIT ((zu32)0x0)

namespace LibChaos {

//! Hash code base class.
class ZHashBase {
public:
    enum hashMethod {
        SIMPLE64,   //!< Simple - fast, dumb, barely a hash function.
        XXHASH64,   //!< xxHash - fast non-cryptographic hash.
        FNV64,      //!< FNV Hash - simple non-cryptographic hash.
        CRC32,      //!< CRC-32 - classic CRC.
        MD5,        //!< MD5 - old cryptographic hash.
        SHA1,       //!< SHA-1 - old cryptographic hash.
        DEFAULT = FNV64,
    };

public:
    virtual ~ZHashBase(){}

protected:
    virtual void feedHash(const zbyte *data, zu64 size) = 0;
    virtual void finishHash(){}
};

//! 32-bit hash code base class.
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

//! 64-bit hash code base class.
class ZHash64Base : public ZHashBase {
public:
    typedef zu64 hashtype;

public:
    ZHash64Base(zu64 hash) : _hash(hash){}

public:
    virtual zu64 hash() const { return _hash; }

public:
    // Simple Hash
    static zu64 simpleHash64_hash(const zbyte *data, zu64 size, zu64 seed = ZHASH_SIMPLEHASH_INIT);

    // FNV Hash
    static zu64 fnvHash64_hash(const zbyte *data, zu64 size, zu64 hval = ZHASH_FNV1A_64_INIT);

    // xxHash
    static zu64 xxHash64_hash(const zbyte *data, zu64 size);
    static void *xxHash64_init();
    static void xxHash64_feed(void *state, const zbyte *data, zu64 size);
    static zu64 xxHash64_done(void *state);

protected:
    hashtype _hash;
};

class ZHashBigBase : public ZHashBase {
public:
    typedef ZBinary hashtype;
public:
    ZHashBigBase(ZBinary hash) : _hash(hash){}

public:
    virtual ZBinary hash() const { return _hash; }

public:
    // MD5
    static ZBinary md5_hash(const zbyte *data, zu64 size);
    static void *md5_init();
    static void md5_feed(void *context, const zbyte *data, zu64 size);
    static ZBinary md5_finish(void *context);

    // SHA
    static ZBinary sha1_hash(const zbyte *data, zu64 size);
    static void *sha1_init();
    static void sha1_feed(void *context, const zbyte *data, zu64 size);
    static ZBinary sha1_finish(void *context);

protected:
    hashtype _hash;
};

// Base ZHashMethod template
//! Hash method provider template.
template <ZHashBase::hashMethod> class ZHashMethod;

// CRC-32 (32-bit)
//! Hash method provider for 32-bit CRC.
template <> class ZHashMethod<ZHashBase::CRC32> : public ZHash32Base {
public:
    ZHashMethod() : ZHash32Base(ZHASH_CRC32_INIT){}
    ZHashMethod(const zbyte *data, zu64 size) : ZHash32Base(crcHash32_hash(data, size)){}
protected:
    void feedHash(const zbyte *data, zu64 size){
        _hash = crcHash32_hash(data, size, _hash);
    }
};

// Simple Hash (64-bit)
//! Hash method provider for 64-bit simple hash.
template <> class ZHashMethod<ZHashBase::SIMPLE64> : public ZHash64Base {
public:
    ZHashMethod() : ZHash64Base(ZHASH_SIMPLEHASH_INIT){}
    ZHashMethod(const zbyte *data, zu64 size) : ZHash64Base(simpleHash64_hash(data, size)){}
protected:
    void feedHash(const zbyte *data, zu64 size){
        _hash = simpleHash64_hash(data, size, _hash);
    }
};

// FNV-1a Hash (64-bit)
//! Hash method provider for 64-bit FNV-1a hash.
template <> class ZHashMethod<ZHashBase::FNV64> : public ZHash64Base {
public:
    ZHashMethod() : ZHash64Base(ZHASH_FNV1A_64_INIT){}
    ZHashMethod(const zbyte *data, zu64 size) : ZHash64Base(fnvHash64_hash(data, size)){}
protected:
    void feedHash(const zbyte *data, zu64 size){
        _hash = fnvHash64_hash(data, size, _hash);
    }
};

// xxHash (64-bit)
//! Hash method provider for 64-bit XXH64 hash.
template <> class ZHashMethod<ZHashBase::XXHASH64> : public ZHash64Base {
public:
    ZHashMethod() : ZHash64Base(0), _state(xxHash64_init()){}
    ZHashMethod(const zbyte *data, zu64 size) : ZHash64Base(xxHash64_hash(data, size)), _state(nullptr){}
protected:
    void feedHash(const zbyte *data, zu64 size){
        if(_state != nullptr)
            xxHash64_feed(_state, data, size);
    }
    void finishHash(){
        if(_state != nullptr)
            _hash = xxHash64_done(_state);
        _state = nullptr;
    }
protected:
    void *_state;
};

#ifdef LIBCHAOS_HAS_CRYPTO

// MD5 (128-bit)
//! Hash method provider for 128-bit MD5 hash.
template <> class ZHashMethod<ZHashBase::MD5> : public ZHashBigBase {
public:
    ZHashMethod() : ZHashBigBase(ZBinary()), _context(md5_init()){}
    ZHashMethod(const zbyte *data, zu64 size) : ZHashBigBase(md5_hash(data, size)), _context(nullptr){}
protected:
    void feedHash(const zbyte *data, zu64 size){
        if(_context != nullptr)
            md5_feed(_context, data, size);
    }
    void finishHash(){
        if(_context != nullptr)
            _hash = md5_finish(_context);
        _context = nullptr;
    }
protected:
    void *_context;
};

// SHA-1 (160-bit)
//! Hash method provider for 160-bit SHA-1 hash.
template <> class ZHashMethod<ZHashBase::SHA1> : public ZHashBigBase {
public:
    ZHashMethod() : ZHashBigBase(ZBinary()), _context(sha1_init()){}
    ZHashMethod(const zbyte *data, zu64 size) : ZHashBigBase(sha1_hash(data, size)), _context(nullptr){}
protected:
    void feedHash(const zbyte *data, zu64 size){
        if(_context != nullptr)
            sha1_feed(_context, data, size);
    }
    void finishHash(){
        if(_context != nullptr)
            _hash = sha1_finish(_context);
        _context = nullptr;
    }
protected:
    void *_context;
};

#endif

// Base ZHash template <data type T, hash method M, SFINAE placeholder>
//! Hash code generator template.
template <typename T, ZHashBase::hashMethod M = ZHashBase::DEFAULT, typename = void> class ZHash;

// zu64 specialization
//! Hash code template specialization for zu64.
template <> class ZHash<zu64> : public ZHash64Base {
public:
    ZHash(zu64 data) : ZHash64Base(data){}
protected:
    void feedHash(const zbyte *data, zu64 size){}
};

// enum specialization
template <typename T> class ZHash<T, ZHashBase::DEFAULT, typename std::enable_if<std::is_enum<T>::value>::type> : public ZHash<zu64> {
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
template <> class ZHash<TYPE, ZHashBase::DEFAULT> : public ZHashMethod<ZHashBase::DEFAULT> { \
public: \
    ZHash ARGS : ZHashMethod<ZHashBase::DEFAULT> CONSTRUCT IMPLEMENTATION \
};

// ZBinary specialization ZHash
ZHASH_USER_SPECIALIAZATION(ZBinary, (const ZBinary &bin), (bin.raw(), bin.size()), {})

// ZString specialization ZHash
ZHASH_USER_SPECIALIAZATION(ZString, (const ZString &str), (str.bytes(), str.size()), {})

}

#endif // ZHASH
