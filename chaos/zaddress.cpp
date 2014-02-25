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

ZAddress::ZAddress() : _addr(0), _port(0){

}
ZAddress::ZAddress(zu8 a, zu8 b, zu8 c, zu8 d, zu16 prt) : _addr((a << 24) | (b << 16) | (c << 8) | d), _port(prt){

}
ZAddress::ZAddress(zu32 add, zu16 prt) : _addr(add), _port(prt){

}
ZAddress::ZAddress(ZString str) : _addr(0), _port(0){
    ArZ addrprt = str.explode(':');
    if(addrprt.size() == 2){
        ArZ addr = addrprt[0].explode('.');
        if(addr.size() == 4){
            _a = (zu8)addr[0].tint();
            _b = (zu8)addr[1].tint();
            _c = (zu8)addr[2].tint();
            _d = (zu8)addr[3].tint();
            _port = (zu16)addrprt[1].tint();
        } else {
            name = addrprt[0];
            _port = (zu16)addrprt[1].tint();
        }
    } else {
        name = str;
    }
}

ZAddress::~ZAddress(){
}

zu32 ZAddress::address() const {
    return _addr;
}
zu8 ZAddress::a() const {
    return _a;
}
zu8 ZAddress::b() const {
    return _b;
}
zu8 ZAddress::c() const {
    return _c;
}
zu8 ZAddress::d() const {
    return _d;
}
zu16 ZAddress::port() const {
    return _port;
}

ZString ZAddress::addrStr() const {
    if(name.isEmpty())
        return (ZString(a()) << "." << b() << '.' << c() << '.' << d());
    else
        return name;
}
ZString ZAddress::portStr() const {
    return ZString(port());
}
ZString ZAddress::fullStr() const {
    return (ZString(addrStr()) << ':' << port());
}

bool ZAddress::getAddrInfo(struct addrinfo *&out) const {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_IDN | AI_CANONNAME | AI_CANONIDN;
    if(_addr == 0)
        hints.ai_flags = hints.ai_flags | AI_PASSIVE;

    if(_addr == 0){
        if(_port == 0){
            return false;
        } else {
            ZString port = portStr();
            int status;
            if((status = getaddrinfo(NULL, port.cc(), &hints, &res)) != 0){
                ELOG("ZSocket: getaddrinfo 1: " << gai_strerror(status));
                return false;
            }
        }
    } else {
        ZString addr = addrStr();
        if(_port == 0){
            int status;
            if((status = getaddrinfo(addr.cc(), NULL, &hints, &res)) != 0){
                ELOG("ZSocket: getaddrinfo 2: " << gai_strerror(status));
                return false;
            }
        } else {
            ZString port = portStr();
            int status;
            if((status = getaddrinfo(addr.cc(), port.cc(), &hints, &res)) != 0){
                ELOG("ZSocket: getaddrinfo 3: " << gai_strerror(status));
                return false;
            }
        }
    }

//    for(struct addrinfo *p = res; p != NULL; p = p->ai_next) {
//        void *addr;
//        ZString ipver;
//        char ipstr[INET6_ADDRSTRLEN];

//        if (p->ai_family == AF_INET) {
//            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
//            addr = &(ipv4->sin_addr);
//            ipver = "IPv4";
//        } else {
//            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
//            addr = &(ipv6->sin6_addr);
//            ipver = "IPv6";
//        }

//        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
//    }

    //addressinfo = res;
    out = res;
    return true;
}

void ZAddress::freeAddrInfo(struct addrinfo *old){
    freeaddrinfo(old);
}

}
