#ifndef ZADDRESS_H
#define ZADDRESS_H

#include "ztypes.h"
#include "zstring.h"

#include <sys/types.h>

#if PLATFORM == WINDOWS
    #include <winsock2.h>
#elif PLATFORM == LINUX
    #include <sys/socket.h>
    #include <netdb.h>
    #include <arpa/inet.h>
#endif

namespace LibChaos {

typedef zu16 zport;

class ZAddress {
public:
    ZAddress();
    ZAddress(zu8 a, zu8 b, zu8 c, zu8 d, zport port);
    ZAddress(zu32 add, zport port);
    ZAddress(ZString str);

    ZAddress(const ZAddress &other);

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

    bool doLookup();

    sockaddr *getSockAddr() const;
    socklen_t getSockAddrLen() const;

private:

    union {
        zu8 _u_addr_ptr_base;

        // IPv6 128 bits
        struct {
            zu64 _v6_first  : 64;
            zu64 _v6_second : 64;
        };

        // IPv4 32 bits
        union {
            zu32 _v4_addr : 32;
            struct {
                zu8 _v4_d : 8;
                zu8 _v4_c : 8;
                zu8 _v4_b : 8;
                zu8 _v4_a : 8;
            };
        };
    };


    zport _port;

    ZString name;

    sockaddr_storage *_sockaddress;
    socklen_t _socksize;
};

}

#endif // ZADDRESS_H
