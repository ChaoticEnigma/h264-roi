#ifndef ZADDRESS_H
#define ZADDRESS_H

#include "ztypes.h"
#include "zstring.h"

#include <sys/types.h>

#if PLATFORM == WINDOWS
    #include <winsock2.h>
    #define IPV4_MAX 16
    #define IPV6_MAX 46
#elif PLATFORM == LINUX
    #include <sys/socket.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #define IPV4_MAX INET_ADDRSTRLEN
    #define IPV6_MAX INET6_ADDRSTRLEN
#endif

namespace LibChaos {

typedef zu16 zport;

class ZAddress {
public:
    enum address_type {
        ipv4 = AF_INET,
        ipv6 = AF_INET6,
        hostname = 3
    };

    ZAddress();
    ZAddress(ZString str);
    ZAddress(ZString str, zport port);
    ZAddress(int protocol, ZString str);

    //ZAddress(zu8 a, zu8 b, zu8 c, zu8 d, zport port);
    //ZAddress(zu32 add, zport port);
    ZAddress(zport port);

    ZAddress(const sockaddr_storage *);

    ZAddress(const ZAddress &other);

    ~ZAddress();

    inline bool operator==(const ZAddress &rhs) const {
        if(     this->_protocol == rhs._protocol &&
                this->_name == rhs._name &&
                this->_v6_first == rhs._v6_first &&
                this->_v6_second == rhs._v6_second &&
                this->_port == rhs._port
            )
            return true;
        return false;
    }
    inline bool operator!=(const ZAddress &rhs) const {
        return !operator==(rhs);
    }

    // Get IPv4 parts
    zu32 address() const;
    zu8 a() const;
    zu8 b() const;
    zu8 c() const;
    zu8 d() const;

    zport port() const;

    int type() const;
    bool isName() const {
        return _protocol == hostname;
    }

    ZString str() const;

    ZAddress doLookup() const;
    static ZArray<ZAddress> lookUpName(ZString name);

    bool populate(sockaddr_storage *ptr) const;

private:
    void parseAny(ZString);
    bool parseIP(int, ZString);

    //static ZString strIP(int af, const void *ptr);

private:
    int _protocol;
    ZString _name;

    union {
        // IPv6 128 bits
        union {
            zu8 _v6_addr[16];
            struct {
                zu64 _v6_second;
                zu64 _v6_first;
            };
        };

        // IPv4 32 bits
        union {
            zu8 _v4_addr[4];
            zu32 _v4_addr_32;
            struct {
                zu8 _v4_a;
                zu8 _v4_b;
                zu8 _v4_c;
                zu8 _v4_d;
            };
        };
    };
    zport _port;
};

}

#endif // ZADDRESS_H
