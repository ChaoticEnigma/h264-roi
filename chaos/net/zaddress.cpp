/*******************************************************************************
**                                  LibChaos                                  **
**                                zaddress.cpp                                **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zaddress.h"
#include "zlog.h"
#include "zerror.h"

#ifdef ZSOCKET_WINAPI
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <string.h>
    #include <netinet/in.h>
#endif

#include "zsocket.h"

namespace LibChaos {

//ZAddressData::ZAddressData(int fam, int typ, int pro, zport port) : _family(fam), _type(typ), _protocol(pro), _port(port){}
ZAddressData::ZAddressData(int fam, int typ, int pro, zport port) : _family(fam), _port(port){

}

//ZAddressData::ZAddressData(const ZAddressData &other) : _family(other._family), _type(other._type), _protocol(other._protocol), _name(other._name), _port(other._port){
ZAddressData::ZAddressData(const ZAddressData &other) : _family(other._family), _name(other._name), _port(other._port){
    memcpy(_v6_addr, other._v6_addr, sizeof(_v6_addr));
}

// ///////////////////////////////////////////////////////////////////////////////////////////////////
// ZADDRESS
// ///////////////////////////////////////////////////////////////////////////////////////////////////

#if COMPILER == MINGW || PLATFORM == CYGWIN
const char *inet_ntop(int af, const void *src, char *dest, int cnt){
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

    if(WSAAddressToStringA((struct sockaddr *)&srcaddr, sizeof(sockaddr_storage), 0, dest, (LPDWORD)&cnt) != 0){
        ELOG("ZAddress: WSAAddressToString error " << ZError::getSystemError());
        return NULL;
    }
    return dest;
}

int inet_pton(int af, const char *src, void *dst){
    struct sockaddr_storage ss;
    int size = sizeof(ss);
    char src_copy[INET6_ADDRSTRLEN + 1];

    memset(&ss, 0, sizeof(sockaddr_storage));
    // Stupid windows non-const API
    strncpy(src_copy, src, INET6_ADDRSTRLEN + 1);
    src_copy[INET6_ADDRSTRLEN] = 0;

    if(WSAStringToAddressA(src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0){
        switch(af) {
        case AF_INET:
            *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
            return 1;
        case AF_INET6:
            *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
            return 1;
        default:
            return 0;
        }
    }
    return 0;
}
#endif

ZAddress::ZAddress() : ZAddressData(IPV4, 0, 0, 0){
    memset(_v6_addr, 0, 16);
}

ZAddress::ZAddress(ZString str) : ZAddressData(IPV4, 0, 0, 0){
    _parseAny(str);
}
ZAddress::ZAddress(ZString str, zport port) : ZAddressData(IPV4, 0, 0, port){
    _parseAny(str);
}
ZAddress::ZAddress(int fam, ZString str) : ZAddressData(fam, 0, 0, 0){
    if(_family == IPV4){
        _parseIP(IPV4, str);
    } else if(_family == IPV6){
        _parseIP(IPV6, str);
    } else {
        _family = NAME;
        _name = str;
    }
}

// IPv4
//ZAddress::ZAddress(zu8 a, zu8 b, zu8 c, zu8 d, zu16 prt) : _protocol(ipv4), _v4_a(a), _v4_b(b), _v4_c(c), _v4_d(d), _port(prt){}
//ZAddress::ZAddress(zu32 add, zu16 prt) : _protocol(ipv4), _v4_addr_32(add), _port(prt){}

ZAddress::ZAddress(zport port) : ZAddressData(IPV4, 0, 0, port){
    memset(_v6_addr, 0, 16);
}

ZAddress::ZAddress(const sockaddr_storage *addr, socklen_t len) : ZAddressData(IPV4, 0, 0, 0){
    if(addr->ss_family == IPV4){
        if(len >= sizeof(sockaddr_in)){
            const sockaddr_in *v4 = (const sockaddr_in *)addr;
            _family = v4->sin_family;
            memcpy(_v4_addr, &(v4->sin_addr), sizeof(v4->sin_addr));
            _port = v4->sin_port;
        } else {
            ELOG("sockaddr too small");
        }

    } else if(addr->ss_family == IPV6){
        if(len >= sizeof(sockaddr_in6)){
            const sockaddr_in6 *v6 = (const sockaddr_in6 *)addr;
            _family = v6->sin6_family;
            memcpy(_v6_addr, &(v6->sin6_addr), sizeof(v6->sin6_addr));
            _port = v6->sin6_port;
        } else {
            ELOG("sockaddr too small");
        }
    }
}

ZAddress::ZAddress(const sockaddr *sa) : ZAddressData(IPV4, 0, 0, 0){
    if(sa->sa_family == AF_INET){
        const sockaddr_in *v4 = (const sockaddr_in *)sa;
        _family = v4->sin_family;
        memcpy(_v4_addr, &(v4->sin_addr), sizeof(v4->sin_addr));
        _port = v4->sin_port;
    } else if(sa->sa_family == AF_INET6){
        const sockaddr_in6 *v6 = (const sockaddr_in6 *)sa;
        _family = v6->sin6_family;
        memcpy(_v6_addr, &(v6->sin6_addr), sizeof(v6->sin6_addr));
        _port = v6->sin6_port;
    }
}

ZAddress::~ZAddress(){

}

ZAddress &ZAddress::operator=(ZAddress rhs){
    _family = rhs._family;
    //_type = rhs._type;
    //_protocol = rhs._protocol;
    _port = rhs._port;
    _name = rhs._name;
    memcpy(_v6_addr, rhs._v6_addr, sizeof(_v6_addr));
    return *this;
}

bool ZAddress::operator==(const ZAddress &rhs) const {
    if(     this->_family == rhs._family &&
            //this->_type == rhs._type &&
            //this->_protocol == rhs._protocol &&
            this->_name == rhs._name &&
            this->_v6_parts.first == rhs._v6_parts.first &&
            this->_v6_parts.second == rhs._v6_parts.second &&
            this->_port == rhs._port
            )
        return true;
    return false;
}

ZString ZAddress::familyStr() const {
    switch(_family){
    case ZAddress::NAME:    return "Unspecified";
    case ZAddress::UNIX:        return "UNIX";
    case ZAddress::IPV4:        return "IPv4";
    case ZAddress::IPV6:        return "IPv6";
    default:                    return "Unknwon";
    }
}

//ZString ZAddress::typeStr() const {
//    switch(_type){
//        case ZSocket::STREAM:   return "Stream";
//        case ZSocket::DATAGRAM: return "Datagram";
//        case ZSocket::RAW:      return "Raw";
//        default:                return "Unknown";
//    }
//}

//ZString ZAddress::protocolStr() const {
//    switch(_protocol){
//        case ZAddress::IP:  return "IP";
//        case ZAddress::TCP: return "TCP";
//        case ZAddress::UDP: return "UDP";
//        default:            return "Unknown";
//    }
//}

ZString ZAddress::str() const {
    socklen_t csz;
    if(_family == IPV4){
        csz = IPV4_MAX;
    } else if(_family == IPV6){
        csz = IPV6_MAX;
    } else {
        return _name;
    }

    char *str = new char[csz];
#if COMPILER == MSVC
    inet_ntop(_family, (void *)&_v4_addr, str, csz);
#else
    inet_ntop(_family, (const void *)&_v4_addr, str, csz);
#endif
    ZString out = str;
    delete[] str;
    return out;
}

bool ZAddress::populate(sockaddr_storage *ptr) const {
    //bzero((char*)ptr, sizeof(sockaddr_storage));
    memset(ptr, 0, sizeof(sockaddr_storage));
    if(_family == IPV4){
        sockaddr_in *v4 = (sockaddr_in *)ptr;
        v4->sin_family = AF_INET;
        v4->sin_port = htons(_port);
        memcpy(&(v4->sin_addr.s_addr), _v4_addr, sizeof(v4->sin_addr.s_addr));
    } else if(_family == IPV6){
        sockaddr_in6 *v6 = (sockaddr_in6 *)ptr;
        v6->sin6_family = AF_INET6;
        v6->sin6_port = htons(_port);
        memcpy(&(v6->sin6_addr), _v6_addr, sizeof(v6->sin6_addr));
    } else {
        return false;
    }
    return true;
}

socklen_t ZAddress::getSockLen() const {
    if(_family == IPV4)
        return sizeof(sockaddr_in);
    else if(_family == IPV6)
        return sizeof(sockaddr_in6);
    else
        return 0;
}

void ZAddress::_parseAny(ZString str){
    memset(_v6_addr, 0, 16);
    if(!str.isEmpty()){
        // Try IPv4
        if(!_parseIP(IPV4, str)){
            // Try IPv6
            if(!_parseIP(IPV6, str)){
                // Store name
                _family = NAME;
                _name = str;
            }
        }
    }
}

bool ZAddress::_parseIP(int af, ZString str){
    if(af == IPV4){
        sockaddr_in addr4;
        int status = inet_pton(AF_INET, str.cc(), &(addr4.sin_addr));
        if(status == 0){
            // Invalid IPv4
            return false;
        } else if(status == 1){
            // IPv4 address
            _family = IPV4;
            memcpy(_v4_addr, &(addr4.sin_addr), sizeof(addr4.sin_addr));
        } else {
            // Internal error
            ELOG("ZAddress: parseIPv4 error " << ZError::getSystemError());
            return false;
        }
        return true;
    } else if(af == IPV6){
        sockaddr_in6 addr6;
        int status = inet_pton(AF_INET6, str.cc(), &(addr6.sin6_addr));
        if(status == 0){
            // Invalid address, store as string
            return false;
        } else if(status == 1){
            // IPv6 address
            _family = IPV6;
            memcpy(_v6_addr, &(addr6.sin6_addr), sizeof(addr6.sin6_addr));
        } else {
            // Internal error
            ELOG("ZAddress: parseIPv6 error " << ZError::getSystemError());
            return false;
        }
        return true;
    }
    return false;
}

}

