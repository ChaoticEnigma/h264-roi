#ifndef ZADDRESS_H
#define ZADDRESS_H

#include "ztypes.h"
#include "zstring.h"

namespace LibChaos {

typedef zu16 zport;

class ZAddress {
public:
    ZAddress();
    ZAddress(zu8 a, zu8 b, zu8 c, zu8 d, zport port);
    ZAddress(zu32 add, zport port);
    ZAddress(ZString str);

    zu32 address() const;
    zu8 a() const;
    zu8 b() const;
    zu8 c() const;
    zu8 d() const;
    zport port() const;
    ZString str() const;

//    bool operator ==(const ZAddress &other) const;
//    bool operator !=(const ZAddress &other) const;
private:
    union {
        zu32 _addr : 32;
        struct {
            zu8 _d : 8;
            zu8 _c : 8;
            zu8 _b : 8;
            zu8 _a : 8;
        };
    };
    zport _port;
};

}

#endif // ZADDRESS_H
