#include "zaddress.h"

#include <string.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netdb.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>
#include <unistd.h>

#if PLATFORM == WINDOWS
    #include <windows.h>
#endif

namespace LibChaos {

ZAddress::ZAddress() : _addr(0), _port(0){

}
ZAddress::ZAddress(zu8 a, zu8 b, zu8 c, zu8 d, zu16 prt) : _addr((a << 24) | (b << 16) | (c << 8) | d), _port(prt){

}
ZAddress::ZAddress(zu32 add, zu16 prt) : _addr(add), _port(prt){
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    getaddrinfo(NULL, "3490", &hints, &res);
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
        }
    }
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
ZString ZAddress::str() const {
    return (ZString() << a() << "." << b() << '.' << c() << '.' << d() << ':' << port());
}

//bool ZAddress::operator ==(const ZAddress &other) const {
//    return _addr == other._addr && _port == other._port;
//}
//bool ZAddress::operator !=(const ZAddress &other) const {
//    return !(*this == other);
//}

}
