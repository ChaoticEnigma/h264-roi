#ifndef ZSOCKET_H
#define ZSOCKET_H

#include "ztypes.h"
#include "zstring.h"

#include <netinet/in.h>

namespace LibChaos {

class ZAddress {
public:
    ZAddress() : a(0), b (0), c(0), d(0){}
    ZAddress(ZString addr, zu16 port){
        ArZ ad = addr.explode('.');
        if(ad.size() == 4){
            a = (unsigned char)ad[0].tint();
            b = (unsigned char)ad[1].tint();
            c = (unsigned char)ad[2].tint();
            d = (unsigned char)ad[3].tint();
            _port = port;
        }
    }
    ZAddress(zu32 aaddr, zu16 aport) : addr(aaddr), _port(aport){}
    ZAddress(zu8 aa, zu8 ab, zu8 ac, zu8 ad, zu16 aport) : a(aa), b (ab), c(ac), d(ad), _port(aport){}

    zu32 address(){
        return addr;
    }
    zu16 port(){
        return _port;
    }

private:
    union {
        zu32 addr : 32;
        struct {
            zu8 a : 8;
            zu8 b : 8;
            zu8 c : 8;
            zu8 d : 8;
        };
    };
    zu16 _port : 16;
};

class ZSocket {
public:
    enum socket_type {
        tcp = SOCK_STREAM,
        udp = SOCK_DGRAM,
        raw = SOCK_RAW
    };
    enum socket_family {
        ipv4 = AF_INET,
        unix = AF_UNIX,
        ipv6 = AF_INET6
    };

    typedef void (*receiveCallback)(ZAddress, ZString);

    ZSocket();

    bool open(socket_type typ, socket_family chn, int port);
    bool isOpen();
    void close();

    bool send(ZAddress addr, const ZString &data);

    zu64 receive(ZAddress &, ZString &);
    bool listen(receiveCallback);

private:
    int socketHandle;
    bool isopen;
    socket_type type;
    socket_family family;
    int port;
};

}

#endif // ZSOCKET_H
