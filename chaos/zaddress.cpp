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

ZAddress::ZAddress() : _v4_addr(0), _port(0), _sockaddress(new sockaddr_storage){

}
ZAddress::ZAddress(zu8 a, zu8 b, zu8 c, zu8 d, zu16 prt) : _v4_addr((a << 24) | (b << 16) | (c << 8) | d), _port(prt), _sockaddress(new sockaddr_storage){

}
ZAddress::ZAddress(zu32 add, zu16 prt) : _v4_addr(add), _port(prt), _sockaddress(new sockaddr_storage){

}
ZAddress::ZAddress(ZString str) : _v4_addr(0), _port(0), _sockaddress(new sockaddr_storage){
    ArZ addrprt = str.explode(':');
    if(addrprt.size() == 2){
        ArZ addr = addrprt[0].explode('.');
        if(addr.size() == 4){
            _v4_a = (zu8)addr[0].tint();
            _v4_b = (zu8)addr[1].tint();
            _v4_c = (zu8)addr[2].tint();
            _v4_d = (zu8)addr[3].tint();
            _port = (zu16)addrprt[1].tint();
        } else {
            name = addrprt[0];
            _port = (zu16)addrprt[1].tint();
        }
    } else {
        name = str;
    }
}

ZAddress::ZAddress(const ZAddress &other) : _v4_addr(other._v4_addr), _port(other._port), _sockaddress(new sockaddr_storage){
    memcpy(_sockaddress, other._sockaddress, other._socksize);
}

ZAddress::~ZAddress(){
    delete _sockaddress;
}

zu32 ZAddress::address() const {
    return _v4_addr;
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

bool ZAddress::doLookup(){
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_flags = AI_IDN | AI_CANONNAME | AI_CANONIDN;
    if(_v4_addr == 0)
        hints.ai_flags = hints.ai_flags | AI_PASSIVE;

    ZString addr = addrStr();
    ZString port = portStr();

    char *addrptr;
    char *portptr;

    if(!addr.isEmpty() || _v4_addr != 0){
        addrptr = (char *)addr.cc();
    } else {
        addrptr = NULL;
    }
    if(_port != 0){
        portptr = (char *)port.cc();
    } else {
        portptr = NULL;
    }

    int status;
    if((status = getaddrinfo(addrptr, portptr, &hints, &res)) != 0){
        ELOG("ZSocket: getaddrinfo 1: " << gai_strerror(status));
        return false;
    }

    memcpy(_sockaddress, res->ai_addr, res->ai_addrlen); // Expand later to take more than the first address

    freeaddrinfo(res);
    return true;
}

sockaddr *ZAddress::getSockAddr() const {
    return (sockaddr *)_sockaddress;
}

socklen_t ZAddress::getSockAddrLen() const {
    return _socksize;
}

}
