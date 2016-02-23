/*******************************************************************************
**                                  LibChaos                                  **
**                                 zaddress.h                                 **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZADDRESS_H
#define ZADDRESS_H

#include "ztypes.h"
#include "zstring.h"

#if PLATFORM == WINDOWS || PLATFORM == CYGWIN
    #define ZSOCKET_WINAPI
#endif

#ifdef ZSOCKET_WINAPI
    #include <winsock2.h>
    #define IPV4_MAX 16
    #define IPV6_MAX 46
#else
    #if PLATFORM == MACOSX
        #include <sys/types.h>
        #include <arpa/inet.h>
        //#include <netdb.h>
    #endif
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
    //int _type;
    //int _protocol;

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

struct SockAddr;

class ZAddress : private ZAddressData {
public:
    enum address_family {
        HOSTNAME = AF_UNSPEC,
        UNIX = AF_UNIX,
        IPV4 = AF_INET,
        IPV6 = AF_INET6,
    };

    enum protocol_type {
        IP = IPPROTO_IP,
        TCP = IPPROTO_TCP,
        UDP = IPPROTO_UDP,
    };

public:
    ZAddress();
    ZAddress(ZString str);
    ZAddress(ZString str, zport port);
    ZAddress(int protocol, ZString str);

    //ZAddress(zu8 a, zu8 b, zu8 c, zu8 d, zport port);
    //ZAddress(zu32 add, zport port);
    ZAddress(zport port);

    ZAddress(const sockaddr_storage *);
    ZAddress(const sockaddr *sa);

    ~ZAddress();

    ZAddress &operator=(ZAddress rhs);

    inline bool operator==(const ZAddress &rhs) const {
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
    inline bool operator!=(const ZAddress &rhs) const {
        return !operator==(rhs);
    }

    zport port() const {
        return _port;
    }
    void setPort(zport port){
        _port = port;
    }

    int family() const {
        return _family;
    }
    bool isName() const {
        return _family == HOSTNAME;
    }
    ZString familyStr() const;

//    int type() const {
//        return _type;
//    }
//    void setType(int set){
//        _type = set;
//    }
//    ZString typeStr() const;

//    int protocol() const {
//        return _protocol;
//    }
//    void setProtocol(int set){
//        _protocol = set;
//    }
//    ZString protocolStr() const;

    ZString str() const;

    static ZArray<SockAddr> lookUp(ZAddress name);

    /*! Populate the sockaddr_storage struct \a ptr with the necessary values from this ZAddress.
     *  \note \a ptr is zeroed before population.
     */
    bool populate(sockaddr_storage *ptr) const;

    //! Get a string describing the address in the form [addr]:port,family,type,protocol.
    ZString debugStr() const {
        //return ZString() + '[' + str() + "]:" + port() + ',' + family() + '-' + type() + '-' + protocol();
        return ZString() + '[' + str() + "]:" + port() + ',' + familyStr();
    }

private:
    void parseAny(ZString);
    bool parseIP(int, ZString);

    //static ZString strIP(int af, const void *ptr);
};

struct SockAddr {
    ZAddress addr;
    int type;
    int proto;
};

}

#endif // ZADDRESS_H
