#include "zaddress.h"
#include "zlog.h"

#include <string.h>

#include <sys/types.h>

#if PLATFORM == WINDOWS
    #include <winsock2.h>
#elif PLATFORM == LINUX
    #include <sys/socket.h>
    #include <netdb.h>
    #include <arpa/inet.h>
#endif

namespace LibChaos {

ZAddress::ZAddress() : _protocol(ipv4), _port(0){
    memset(_v6_addr, 0, 16);
}

ZAddress::ZAddress(ZString str) : _protocol(ipv4), _port(0){
    parseAny(str);
}
ZAddress::ZAddress(ZString str, zport port) : _protocol(ipv4), _port(port){
    parseAny(str);
}
ZAddress::ZAddress(address_type protocol, ZString str) : _protocol(protocol), _port(0){
    if(protocol == ipv4){
        parseIPv4(str);
    } else if(protocol == ipv6){
        parseIPv6(str);
    } else {
        _protocol = hostname;
        _name = str;
    }
}

// IPv4
ZAddress::ZAddress(zu8 a, zu8 b, zu8 c, zu8 d, zu16 prt) : _protocol(ipv4), _v4_a(a), _v4_b(b), _v4_c(c), _v4_d(d), _port(prt){}
ZAddress::ZAddress(zu32 add, zu16 prt) : _protocol(ipv4), _v4_addr_32(add), _port(prt){}

ZAddress::ZAddress(const ZAddress &other) : _protocol(other._protocol), _port(other._port){
    memcpy(_v6_addr, other._v6_addr, sizeof(_v6_addr));
}

ZAddress::~ZAddress(){
    //delete _sockaddress;
}

zu32 ZAddress::address() const {
    return _v4_addr_32;
}
zu8 ZAddress::a() const {
    return _v4_a;
}
zu8 ZAddress::b() const {
    return _v4_b;
}
zu8 ZAddress::c() const {
    return _v4_c;
}
zu8 ZAddress::d() const {
    return _v4_d;
}
zu16 ZAddress::port() const {
    return _port;
}

ZAddress::address_type ZAddress::type() const {
    return _protocol;
}

ZString ZAddress::str() const {
    if(_protocol == ipv4){
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &_v4_addr, str, INET_ADDRSTRLEN);
        return ZString(str);
    } else if(_protocol == ipv6){
        char str[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &_v6_addr, str, INET6_ADDRSTRLEN);
        return ZString(str);
    } else {
        return _name;
    }
}

ZAddress ZAddress::doLookup() const {
    return lookUpName(this->str())[0];
}

ZArray<ZAddress> ZAddress::lookUpName(ZString name){
    struct addrinfo hints, *res;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    //hints.ai_family = AF_UNSPEC; // Redundant
    //hints.ai_socktype = SOCK_STREAM;

    if((status = getaddrinfo(name.cc(), NULL, &hints, &res)) != 0){
        ELOG("ZSocket: getaddrinfo: " << gai_strerror(status));
        return ZArray<ZAddress>();
    }

    ZArray<ZAddress> addrs;
    for(struct addrinfo *p = res; p != NULL; p = p->ai_next){
        void *addr;
        if(p->ai_family == AF_INET){
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));

        // Other data:
        //      p->ai_family    // Family (AF_INET, AF_INET6)
        //      p->ai_socktype  // Socket type (SOCK_STREAM, SOCK_DGRAM)
        //      p->ai_protocol
        //      p->ai_canonname // Offical service name

        ZAddress newaddr(ipstr);
        if(!addrs.contains(newaddr))
            addrs.push(newaddr);
    }
    freeaddrinfo(res); // free the linked list
    return addrs;
}

void ZAddress::parseAny(ZString str){
    memset(_v6_addr, 0, 16);

    if(!str.isEmpty()){
        // Try IPv4
        if(!parseIPv4(str)){
            // Try IPv6
            if(!parseIPv6(str)){
                // Store name
                _protocol = hostname;
                _name = str;
            }
        }
    }
}
bool ZAddress::parseIPv4(ZString str){
    struct sockaddr_in addr4;
    int status = inet_pton(AF_INET, str.cc(), &(addr4.sin_addr));
    if(status == 0){
        // Invalid IPv4
        return false;
    } else if(status == 1){
        // IPv4 address
        _protocol = ipv4;
        memcpy(_v4_addr, &(addr4.sin_addr), sizeof(sockaddr_in));
    } else {
        // Internal error
        ELOG("ZAddress: parseIPv4 error " << errno << ": " << strerror(errno));
        return false;
    }
    return true;
}
bool ZAddress::parseIPv6(ZString str){
    struct sockaddr_in6 addr6;
    int status = inet_pton(AF_INET6, str.cc(), &(addr6.sin6_addr));
    if(status == 0){
        // Invalid address, store as string
        return false;
    } else if(status == 1){
        // IPv6 address
        _protocol = ipv6;
        memcpy(_v6_addr, &(addr6.sin6_addr), sizeof(sockaddr_in6));
    } else {
        // Internal error
        ELOG("ZAddress: parseIPv6 error " << errno << ": " << strerror(errno));
        return false;
    }
    return true;
}

bool ZAddress::populate(sockaddr_storage *ptr) const {
    if(_protocol == ipv4){
        sockaddr_in *v4 = (sockaddr_in *)ptr;
        v4->sin_family = AF_INET;
        memcpy(&(v4->sin_addr), _v4_addr, sizeof(sockaddr_in));
        v4->sin_port = _port;
    } else if(_protocol == ipv6){
        sockaddr_in6 *v6 = (sockaddr_in6 *)ptr;
        v6->sin6_family = AF_INET6;
        memcpy(&(v6->sin6_addr), _v6_addr, sizeof(sockaddr_in6));
        v6->sin6_port = _port;
    } else {
        return false;
    }
    return true;
}

}
