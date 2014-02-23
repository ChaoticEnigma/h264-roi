#ifndef ZADDRESS_H
#define ZADDRESS_H

#include "ztypes.h"
#include "zstring.h"

namespace LibChaos {

class ZAddress {
public:
    ZAddress();
    ZAddress(zu8 a, zu8 b, zu8 c, zu8 d, zu16 prt);
    ZAddress(zu32 add, zu16 prt);
    ZAddress(ZString str);

    zu32 address() const;
    zu8 a() const;
    zu8 b() const;
    zu8 c() const;
    zu8 d() const;
    zu16 port() const;
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
    zu16 _port;
};

}

#endif // ZADDRESS_H
