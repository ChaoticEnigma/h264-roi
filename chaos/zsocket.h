#ifndef ZSOCKET_H
#define ZSOCKET_H

#include "ztypes.h"
#include "zaddress.h"
#include "zbinary.h"

namespace LibChaos {

class ZSocket {
public:
    typedef void (*receiveCallback)(ZSocket *socket, const ZAddress &sender, const ZBinary &data);

    ZSocket();
    ~ZSocket();

    bool open(zu16 port);
    void close();
    bool isOpen() const;

    bool send(const ZAddress &destination, const ZBinary &data);

    zu32 receive(ZAddress &sender, ZBinary &str);
    void listen(receiveCallback receivedFunc);

private:
    static bool InitializeSockets();
    static void ShutdownSockets();

    int socket;
    unsigned char *buffer;
    static zu64 socket_count;
};

}

#endif // ZSOCKET_H
