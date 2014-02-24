#ifndef ZSOCKET_H
#define ZSOCKET_H

#include "ztypes.h"
#include "zaddress.h"
#include "zbinary.h"

namespace LibChaos {

class ZSocket {
public:
    typedef void (*receiveCallback)(ZSocket *socket, const ZAddress &sender, const ZBinary &data);

//    enum socket_type {
//        tcp = SOCK_STREAM,
//        udp = SOCK_DGRAM,
//        raw = SOCK_RAW
//    };
//    enum socket_family {
//        ipv4 = AF_INET,
//        unix = AF_UNIX,
//        ipv6 = AF_INET6
//    };

    ZSocket();
    ~ZSocket();

    // UDP
    bool open(zport port);
    void close();
    bool isOpen() const;
    bool send(const ZAddress &destination, const ZBinary &data);
    zu32 receive(ZAddress &sender, ZBinary &str);
    void listen(receiveCallback receivedFunc);

    // TCP


private:
    static bool InitializeSockets();
    static void ShutdownSockets();

    static zu64 socket_count;

    int socket;
    unsigned char *buffer;
};

}

#endif // ZSOCKET_H
