#include "zaddress.h"
#include "zlog.h"

#include <string.h>

#include <sys/types.h>

#if PLATFORM == WINDOWS
    #include <winsock2.h>
    #include <ws2tcpip.h>
#elif PLATFORM == LINUX
    #include <sys/socket.h>
    #include <netdb.h>
    #include <arpa/inet.h>
#endif

namespace LibChaos {

#if PLATFORM == WINDOWS
const char *inet_ntop(int af, const void* src, char *dst, int cnt){
    sockaddr_storage srcaddr;
    memset(&srcaddr, 0, sizeof(sockaddr_storage));

    if(af == AF_INET){
        sockaddr_in *v4 = (sockaddr_in *)&srcaddr;
        v4->sin_family = af;
        memcpy(&(v4->sin_addr), src, sizeof(v4->sin_addr));
    } else if(af == AF_INET6){
        sockaddr_in6 *v6 = (sockaddr_in6 *)&srcaddr;
        v6->sin6_family = af;
        memcpy(&(v6->sin6_addr), src, sizeof(v6->sin6_addr));
    } else {
        return NULL;
    }

    if(WSAAddressToString((struct sockaddr *)&srcaddr, sizeof(sockaddr_storage), 0, dst, (LPDWORD) &cnt) != 0){
        ELOG("ZAddress: WSAAddressToString error " << WSAGetLastError());
        return NULL;
    }
    return dst;
}

int inet_pton(int af, const char *src, void *dst){
    struct sockaddr_storage ss;
    int size = sizeof(ss);
    char src_copy[INET6_ADDRSTRLEN+1];

    memset(&ss, 0, sizeof(sockaddr_storage));
    /* stupid non-const API */
    strncpy(src_copy, src, INET6_ADDRSTRLEN+1);
    src_copy[INET6_ADDRSTRLEN] = 0;

    if(WSAStringToAddress(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
        switch(af) {
        case AF_INET:
            *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
            return 1;
        case AF_INET6:
            *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
            return 1;
        }
    }
    return 0;
}
#endif

ZAddress::ZAddress() : _protocol(ipv4), _port(0){
    memset(_v6_addr, 0, 16);
}

ZAddress::ZAddress(ZString str) : _protocol(ipv4), _port(0){
    parseAny(str);
}
ZAddress::ZAddress(ZString str, zport port) : _protocol(ipv4), _port(port){
    parseAny(str);
}
ZAddress::ZAddress(int protocol, ZString str) : _protocol(protocol), _port(0){
    if(protocol == ipv4){
        parseIP(ipv4, str);
    } else if(protocol == ipv6){
        parseIP(ipv6, str);
    } else {
        _protocol = hostname;
        _name = str;
    }
}

// IPv4
//ZAddress::ZAddress(zu8 a, zu8 b, zu8 c, zu8 d, zu16 prt) : _protocol(ipv4), _v4_a(a), _v4_b(b), _v4_c(c), _v4_d(d), _port(prt){}
//ZAddress::ZAddress(zu32 add, zu16 prt) : _protocol(ipv4), _v4_addr_32(add), _port(prt){}

ZAddress::ZAddress(zport port) : _protocol(ipv4), _port(0){
    memset(_v6_addr, 0, 16);
}

ZAddress::ZAddress(const sockaddr_storage *ptr) : _protocol(ipv4), _port(0){
    if(ptr->ss_family == ipv4){
        _protocol = ipv4;
        const sockaddr_in *v4 = (sockaddr_in *)ptr;
        memcpy(_v4_addr, &(v4->sin_addr), sizeof(v4->sin_addr));
        _port = v4->sin_port;
    } else if(ptr->ss_family == ipv6){
        _protocol = ipv6;
        const sockaddr_in6 *v6 = (sockaddr_in6 *)ptr;
        memcpy(_v6_addr, &(v6->sin6_addr), sizeof(v6->sin6_addr));
        _port = v6->sin6_port;
    }
}

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

int ZAddress::type() const {
    return _protocol;
}

ZString ZAddress::str() const {
    int csz;
    if(_protocol == ipv4){
        csz = IPV4_MAX;
    } else if(_protocol == ipv6){
        csz = IPV6_MAX;
    } else {
        return _name;
    }

    char str[csz];
//#if PLATFORM == LINUX
    inet_ntop(_protocol, &_v4_addr, str, csz);
//#elif PLATFORM == WINDOWS
//    sockaddr_storage srcaddr;

//    sockaddr_storage *ptr = &srcaddr;
//    memset(ptr, 0, sizeof(sockaddr_storage));
//    if(_protocol == ipv4){
//        sockaddr_in *v4 = (sockaddr_in *)ptr;
//        v4->sin_family = AF_INET;
//        memcpy(&(v4->sin_addr), _v4_addr, sizeof(sockaddr_in));
//    } else if(_protocol == ipv6){
//        sockaddr_in6 *v6 = (sockaddr_in6 *)ptr;
//        v6->sin6_family = AF_INET6;
//        memcpy(&(v6->sin6_addr), _v6_addr, sizeof(sockaddr_in6));
//    }

//    //populate(&srcaddr);
//    if(WSAAddressToString((struct sockaddr*)&srcaddr, sizeof(sockaddr_storage), 0, str, (LPDWORD)&csz) != 0){
//        ELOG("ZAddress: WSAAddressToString error " << WSAGetLastError());
//        return ZString();
//    }
//#endif
    return ZString(str);
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
            struct sockaddr_in *v4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(v4->sin_addr);
        } else {
            struct sockaddr_in6 *v6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(v6->sin6_addr);
        }
        inet_ntop(p->ai_family, addr, ipstr, INET6_ADDRSTRLEN);
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

bool ZAddress::populate(sockaddr_storage *ptr) const {
    memset(ptr, 0, sizeof(sockaddr_storage));
    if(_protocol == ipv4){
        sockaddr_in *v4 = (sockaddr_in *)ptr;
        v4->sin_family = AF_INET;
        memcpy(&(v4->sin_addr), _v4_addr, sizeof(v4->sin_addr));
        v4->sin_port = _port;
    } else if(_protocol == ipv6){
        sockaddr_in6 *v6 = (sockaddr_in6 *)ptr;
        v6->sin6_family = AF_INET6;
        memcpy(&(v6->sin6_addr), _v6_addr, sizeof(v6->sin6_addr));
        v6->sin6_port = _port;
    } else {
        return false;
    }
    return true;
}

void ZAddress::parseAny(ZString str){
    memset(_v6_addr, 0, 16);

    if(!str.isEmpty()){
        // Try IPv4
        if(!parseIP(ipv4, str)){
            // Try IPv6
            if(!parseIP(ipv6, str)){
                // Store name
                _protocol = hostname;
                _name = str;
            }
        }
    }
}
bool ZAddress::parseIP(int af, ZString str){
    if(af == ipv4){
        struct sockaddr_in addr4;
        int status = inet_pton(AF_INET, str.cc(), &(addr4.sin_addr));
        if(status == 0){
            // Invalid IPv4
            return false;
        } else if(status == 1){
            // IPv4 address
            _protocol = ipv4;
            memcpy(_v4_addr, &(addr4.sin_addr), sizeof(addr4.sin_addr));
        } else {
            // Internal error
            ELOG("ZAddress: parseIPv4 error " << errno << ": " << strerror(errno));
            return false;
        }
        return true;
    } else if(af == ipv6){
        struct sockaddr_in6 addr6;
        int status = inet_pton(AF_INET6, str.cc(), &(addr6.sin6_addr));
        if(status == 0){
            // Invalid address, store as string
            return false;
        } else if(status == 1){
            // IPv6 address
            _protocol = ipv6;
            memcpy(_v6_addr, &(addr6.sin6_addr), sizeof(addr6.sin6_addr));
        } else {
            // Internal error
            ELOG("ZAddress: parseIPv6 error " << errno << ": " << strerror(errno));
            return false;
        }
        return true;
    }
    return false;
}

//ZString ZAddress::strIP(int af, const void *ptr){
//    int csz;
//    if(af == ipv4){
//        csz = IPV4_MAX;
//    } else if(af == ipv6){
//        csz = IPV6_MAX;
//    } else {
//        return ZString();
//    }

//    char str[csz];
//#if PLATFORM == LINUX
//    inet_ntop(af, ptr, str, csz);
//#elif PLATFORM == WINDOWS
//    sockaddr_storage srcaddr;
//    memset(&srcaddr, 0, sizeof(sockaddr_storage));
//    if(af == ipv4){
//        sockaddr_in *v4 = (sockaddr_in *)&srcaddr;
//        v4->sin_family = AF_INET;
//        memcpy(&(v4->sin_addr), ptr, sizeof(sockaddr_in));
//    } else if(af == ipv6){
//        sockaddr_in6 *v6 = (sockaddr_in6 *)&srcaddr;
//        v6->sin6_family = AF_INET6;
//        memcpy(&(v6->sin6_addr), ptr, sizeof(sockaddr_in6));
//    }

//    if(WSAAddressToString((struct sockaddr*)&srcaddr, sizeof(sockaddr_storage), 0, str, (LPDWORD)&csz) != 0){
//        ELOG("ZAddress: WSAAddressToString error " << WSAGetLastError());
//        return ZString();
//    }
//#endif
//    return ZString(csz);
//}

}
