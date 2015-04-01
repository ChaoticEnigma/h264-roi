#ifndef ZHASH
#define ZHASH

#include "ztypes.h"
#include <type_traits>

namespace LibChaos {

class ZHashBase {
public:
    enum hashMethod {
        value,
        hashType1,
    };
public:
    ZHashBase(zu64 hashval) : _hash(hashval){}
    inline zu64 hash() const { return _hash; }
    inline zu64 operator()() const { return hash(); }

    zu64 hashData(const zbyte *data, zu64 size, hashMethod method);

protected:
    zu64 _hash;
};

template <typename T, typename = void> class ZHash;

template <typename T> class ZHash<T, typename std::enable_if<std::is_enum<T>::value>::type> : public ZHashBase {
public:
    ZHash(T data) : ZHashBase((zu64)data){}
};

// z unsigned types
template <> class ZHash<zu8> : public ZHashBase {
public:
    ZHash(zu8 data) : ZHashBase((zu64)data){}
};
template <> class ZHash<zu16> : public ZHashBase {
public:
    ZHash(zu16 data) : ZHashBase((zu64)data){}
};
template <> class ZHash<zu32> : public ZHashBase {
public:
    ZHash(zu32 data) : ZHashBase((zu64)data){}
};
template <> class ZHash<zu64> : public ZHashBase {
public:
    ZHash(zu64 data) : ZHashBase((zu64)data){}
};

// z signed types
template <> class ZHash<zs8> : public ZHashBase {
public:
    ZHash(zs8 data) : ZHashBase((zu64)data){}
};
template <> class ZHash<zs16> : public ZHashBase {
public:
    ZHash(zs16 data) : ZHashBase((zu64)data){}
};
template <> class ZHash<zs32> : public ZHashBase {
public:
    ZHash(zs32 data) : ZHashBase((zu64)data){}
};
template <> class ZHash<zs64> : public ZHashBase {
public:
    ZHash(zs64 data) : ZHashBase((zu64)data){}
};

// signed
template <> class ZHash<char> : public ZHashBase {
public:
    ZHash(char data) : ZHashBase((zu64)data){}
};
template <> class ZHash<long> : public ZHashBase {
public:
    ZHash(long data) : ZHashBase((zu64)data){}
};

// unsigned
template <> class ZHash<unsigned long> : public ZHashBase {
public:
    ZHash(unsigned long data) : ZHashBase((zu64)data){}
};

}

#endif // ZHASH
