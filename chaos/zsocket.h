#ifndef ZSOCKET_H
#define ZSOCKET_H

#include "ztypes.h"
#include "zaddress.h"
#include "zbinary.h"

namespace LibChaos {

class ZSocket {
public:
    typedef void (*receiveCallback)(ZAddress, ZBinary);

    ZSocket();
    ~ZSocket();

    bool open(zu16 port);
    void close();
    bool isOpen() const;

    bool send(const ZAddress &destination, const ZBinary &data);

    //zu32 receiveraw(ZAddress & sender, void * data, zu64 size);
    zu32 receive(ZAddress &sender, ZBinary &str);
    void listen(receiveCallback receivedFunc);
    //void listen(receiveCallback receivedFunc, zu64 limit = -1);

    static bool InitializeSockets();
    static void ShutdownSockets();

private:
    //void receiveOne(receiveCallback receivedFunc);

    int socket;
    unsigned char *buffer;
};

}

#endif // ZSOCKET_H
