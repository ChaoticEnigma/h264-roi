#ifndef ZADDRESS_H
#define ZADDRESS_H

#include "ztypes.h"
#include "zstring.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

namespace LibChaos {

typedef zu16 zport;

class ZAddress {
public:
    ZAddress();
    ZAddress(zu8 a, zu8 b, zu8 c, zu8 d, zport port);
    ZAddress(zu32 add, zport port);
    ZAddress(ZString str);
    ~ZAddress();

    zu32 address() const;
    zu8 a() const;
    zu8 b() const;
    zu8 c() const;
    zu8 d() const;
    zport port() const;

    ZString addrStr() const;
    ZString portStr() const;
    ZString fullStr() const;

    bool getAddrInfo(struct addrinfo *&out) const;
    static void freeAddrInfo(struct addrinfo *old);

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

    ZString name;

    //struct addrinfo *addressinfo;
};

}

#endif // ZADDRESS_H
