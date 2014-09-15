#ifndef ZADDRESS_H
#define ZADDRESS_H

#include "ztypes.h"
#include "zstring.h"

#if PLATFORM == WINDOWS
    #include <winsock2.h>
    #define IPV4_MAX 16
    #define IPV6_MAX 46
#elif PLATFORM == LINUX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #define IPV4_MAX INET_ADDRSTRLEN
    #define IPV6_MAX INET6_ADDRSTRLEN
#endif

namespace LibChaos {

typedef zu16 zport;

class ZAddressData {
protected:
    ZAddressData(int fam, int typ, int pro, zport port);
    ZAddressData(const ZAddressData &other);

protected:
    int _family;
    int _type;
    int _protocol;

    ZString _name;

    union {
        // IPv6 128 bits
        union {
            zu8 _v6_addr[16];
            zu16 _v6_blocks[8];
            struct {
                zu64 second;
                zu64 first;
            } _v6_parts;
        };

        // IPv4 32 bits
        union {
            zu8 _v4_addr[4];
            zu32 _v4_addr_32;
            struct {
                zu8 a;
                zu8 b;
                zu8 c;
                zu8 d;
            } _v4_parts;
        };
    };
    zport _port;
};

class ZAddress : private ZAddressData {
public:
    enum address_family {
        ipv4 = AF_INET,
        ipv6 = AF_INET6,
        hostname = AF_UNSPEC,
        unix = AF_UNIX
    };

    enum protocol_type {
        ip = IPPROTO_IP,
        tcp = IPPROTO_TCP,
        udp = IPPROTO_UDP
    };

    ZAddress();
    ZAddress(ZString str);
    ZAddress(ZString str, zport port);
    ZAddress(int protocol, ZString str);

    //ZAddress(zu8 a, zu8 b, zu8 c, zu8 d, zport port);
    //ZAddress(zu32 add, zport port);
    ZAddress(zport port);

    ZAddress(const sockaddr_storage *);

    ZAddress &operator=(ZAddress rhs);

    ~ZAddress();

    inline bool operator==(const ZAddress &rhs) const {
        if(     this->_family == rhs._family &&
                this->_type == rhs._type &&
                this->_protocol == rhs._protocol &&
                this->_name == rhs._name &&
                this->_v6_parts.first == rhs._v6_parts.first &&
                this->_v6_parts.second == rhs._v6_parts.second &&
                this->_port == rhs._port
            )
            return true;
        return false;
    }
    inline bool operator!=(const ZAddress &rhs) const {
        return !operator==(rhs);
    }

    zport port() const;
    void setPort(zport port){
        _port = port;
    }

    int family() const;
    bool isName() const {
        return _family == hostname;
    }

    int type() const {
        return _type;
    }
    void setType(int set){
        _type = set;
    }

    int protocol() const {
        return _protocol;
    }
    void setProtocol(int set){
        _protocol = set;
    }

    ZString str() const;

    static ZArray<ZAddress> lookUp(ZAddress name);

    bool populate(sockaddr_storage *ptr) const;

    ZString debugStr() const {
        return ZString() + '[' + str() + "]:" + _port + ',' + _family + '-' + _type + '-' + _protocol;
    }

private:
    void parseAny(ZString);
    bool parseIP(int, ZString);

    //static ZString strIP(int af, const void *ptr);
};

}

#endif // ZADDRESS_H
