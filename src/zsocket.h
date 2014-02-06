#ifndef ZSOCKET_H
#define ZSOCKET_H

#include "ztypes.h"
#include "zstring.h"

#include <netinet/in.h>

namespace LibChaos {

class ZSocket {
public:
    enum socket_type {
        tcp = SOCK_STREAM,
        udp = SOCK_DGRAM,
        raw = SOCK_RAW
    };
    enum socket_channel {
        ipv4 = AF_INET,
        unix = AF_UNIX,
        ipv6 = AF_INET6
    };

    ZSocket();
    bool open(socket_type, socket_channel);
    void close();

    bool listen(int);
    ZString receive();

private:
    int socketHandle;
    socket_type type;
    socket_channel channel;
    int port;
};

}

#endif // ZSOCKET_H
